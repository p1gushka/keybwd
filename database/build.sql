-- ============================================================
-- build.sql — создаёт схему в базе textdb (предварительно созданной)
-- ============================================================

-- ------------------------------------------------------------
-- 0) Создать пользователя textuser (если ещё не создан)
-- ------------------------------------------------------------
DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_roles WHERE rolname = 'textuser') THEN
        CREATE USER textuser WITH PASSWORD 'secure_password';
    END IF;
END
$$;


-- ------------------------------------------------------------
-- 1) ПОДКЛЮЧЕНИЕ К БАЗЕ
-- ------------------------------------------------------------
\c textdb


-- ------------------------------------------------------------
-- 2) Таблица modes
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS modes (
    id   SERIAL PRIMARY KEY,
    key  VARCHAR(20) UNIQUE NOT NULL,
    name VARCHAR(50) NOT NULL
);
INSERT INTO modes (key, name) VALUES
  ('time',   'На время'),
  ('words',  'По словам'),
  ('quotes', 'Цитаты'),
  ('code',   'Пользовательский код')
ON CONFLICT (key) DO NOTHING;


-- ------------------------------------------------------------
-- 3) Таблица texts (для режимов time и words)
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS texts (
    id         SERIAL PRIMARY KEY,
    mode_id    INTEGER NOT NULL REFERENCES modes(id),
    title      VARCHAR(255),
    content    TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_texts_mode ON texts(mode_id);


-- ------------------------------------------------------------
-- 4) Таблица words (для режима words)
-- ------------------------------------------------------------
DROP TABLE IF EXISTS words CASCADE;

CREATE TABLE words (
    id   SERIAL PRIMARY KEY,
    word VARCHAR(100) NOT NULL
);



-- ------------------------------------------------------------
-- 5) Таблица quotes
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS quotes (
    id         SERIAL PRIMARY KEY,
    content    TEXT NOT NULL,
    length_cat VARCHAR(10) NOT NULL
       CHECK (length_cat IN ('short','medium','long')),
    author     VARCHAR(100),
    created_at TIMESTAMP DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_quotes_length ON quotes(length_cat);


-- ------------------------------------------------------------
-- 6) Таблица code_snippets
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS code_snippets (
    id         SERIAL PRIMARY KEY,
    lang       VARCHAR(20) NOT NULL,
    title      VARCHAR(255),
    content    TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);
CREATE INDEX IF NOT EXISTS idx_code_lang ON code_snippets(lang);


-- ------------------------------------------------------------
-- 7) Таблица players
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS players (
    id             SERIAL PRIMARY KEY,
    username       VARCHAR(50) NOT NULL,
    login          VARCHAR(50) NOT NULL UNIQUE,
    password_hash  TEXT NOT NULL DEFAULT '',
    registered_at  TIMESTAMP DEFAULT NOW()
);


-- ------------------------------------------------------------
-- 8) Таблица games
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS games (
    id               SERIAL PRIMARY KEY,
    player_id        INTEGER NOT NULL REFERENCES players(id) ON DELETE CASCADE,
    played_at        TIMESTAMP NOT NULL DEFAULT NOW(),
    mode             VARCHAR(20) NOT NULL,
    speed_wpm        NUMERIC(5,2) NOT NULL,
    raw_wpm          NUMERIC(5,2) NOT NULL,
    accuracy         NUMERIC(5,2) NOT NULL,
    correct_symbols  INTEGER NOT NULL,
    wrong_symbols    INTEGER NOT NULL,
    missed_symbols   INTEGER NOT NULL,
    extra_symbols    INTEGER NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_games_mode_player_played
  ON games(mode, player_id, played_at DESC);


-- ------------------------------------------------------------
-- 9) Sequence владельцы (без ошибок, если их ещё нет)
-- ------------------------------------------------------------
ALTER SEQUENCE IF EXISTS texts_id_seq   OWNER TO textuser;
ALTER SEQUENCE IF EXISTS players_id_seq OWNER TO textuser;
ALTER SEQUENCE IF EXISTS games_id_seq   OWNER TO textuser;


-- ------------------------------------------------------------
-- 10) Триггер prune_old_games
-- ------------------------------------------------------------
DROP TRIGGER IF EXISTS trg_prune_games ON games;
CREATE OR REPLACE FUNCTION prune_old_games()
RETURNS TRIGGER AS $$
BEGIN
    DELETE FROM games
      WHERE id IN (
        SELECT id
          FROM games
         WHERE player_id = NEW.player_id
         ORDER BY played_at DESC
         OFFSET 5
      );
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER trg_prune_games
  AFTER INSERT ON games
  FOR EACH ROW
  EXECUTE FUNCTION prune_old_games();


-- ------------------------------------------------------------
-- 11) Таблица player_cumulative_stats и триггер update
-- ------------------------------------------------------------
CREATE TABLE IF NOT EXISTS player_cumulative_stats (
    player_id           INTEGER PRIMARY KEY
                          REFERENCES players(id) ON DELETE CASCADE,
    total_games         BIGINT   NOT NULL DEFAULT 0,
    sum_speed_wpm       NUMERIC(20,4) NOT NULL DEFAULT 0,
    sum_raw_wpm         NUMERIC(20,4) NOT NULL DEFAULT 0,
    sum_accuracy        NUMERIC(20,4) NOT NULL DEFAULT 0,
    sum_correct_symbols BIGINT   NOT NULL DEFAULT 0,
    sum_wrong_symbols   BIGINT   NOT NULL DEFAULT 0,
    sum_missed_symbols  BIGINT   NOT NULL DEFAULT 0,
    sum_extra_symbols   BIGINT   NOT NULL DEFAULT 0
);

DROP TRIGGER IF EXISTS trg_update_cumulative ON games;
CREATE OR REPLACE FUNCTION update_cumulative_stats()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO player_cumulative_stats(
        player_id, total_games, sum_speed_wpm, sum_raw_wpm, sum_accuracy,
        sum_correct_symbols, sum_wrong_symbols, sum_missed_symbols, sum_extra_symbols
    ) VALUES (
        NEW.player_id, 1, NEW.speed_wpm, NEW.raw_wpm, NEW.accuracy,
        NEW.correct_symbols, NEW.wrong_symbols, NEW.missed_symbols, NEW.extra_symbols
    )
    ON CONFLICT (player_id) DO UPDATE
    SET
      total_games         = player_cumulative_stats.total_games + 1,
      sum_speed_wpm       = player_cumulative_stats.sum_speed_wpm + NEW.speed_wpm,
      sum_raw_wpm         = player_cumulative_stats.sum_raw_wpm + NEW.raw_wpm,
      sum_accuracy        = player_cumulative_stats.sum_accuracy + NEW.accuracy,
      sum_correct_symbols = player_cumulative_stats.sum_correct_symbols + NEW.correct_symbols,
      sum_wrong_symbols   = player_cumulative_stats.sum_wrong_symbols + NEW.wrong_symbols,
      sum_missed_symbols  = player_cumulative_stats.sum_missed_symbols + NEW.missed_symbols,
      sum_extra_symbols   = player_cumulative_stats.sum_extra_symbols + NEW.extra_symbols;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER trg_update_cumulative
  AFTER INSERT ON games
  FOR EACH ROW
  EXECUTE FUNCTION update_cumulative_stats();


-- ------------------------------------------------------------
-- 12) Материализованные представления и их обновление
-- ------------------------------------------------------------
CREATE MATERIALIZED VIEW IF NOT EXISTS leaderboard_60 AS
SELECT username, speed_wpm, accuracy, played_at
  FROM (
    SELECT DISTINCT ON (g.player_id)
      p.username,
      g.speed_wpm,
      g.accuracy,
      g.played_at
    FROM games g
    JOIN players p ON g.player_id = p.id
   WHERE g.mode = '60'
   ORDER BY g.player_id, g.speed_wpm DESC, g.accuracy DESC, g.played_at DESC
  ) sub
 ORDER BY speed_wpm DESC
 LIMIT 10;

CREATE MATERIALIZED VIEW IF NOT EXISTS leaderboard_15 AS
SELECT username, speed_wpm, accuracy, played_at
  FROM (
    SELECT DISTINCT ON (g.player_id)
      p.username,
      g.speed_wpm,
      g.accuracy,
      g.played_at
    FROM games g
    JOIN players p ON g.player_id = p.id
   WHERE g.mode = '15'
   ORDER BY g.player_id, g.speed_wpm DESC, g.accuracy DESC, g.played_at DESC
  ) sub
 ORDER BY speed_wpm DESC
 LIMIT 10;

CREATE OR REPLACE FUNCTION refresh_leaderboards()
RETURNS VOID AS $$
BEGIN
    REFRESH MATERIALIZED VIEW leaderboard_60;
    REFRESH MATERIALIZED VIEW leaderboard_15;
END;
$$ LANGUAGE plpgsql;


-- ------------------------------------------------------------
-- 13) Вставка текстов
-- ------------------------------------------------------------

-- Для режима 'time' (На время)
INSERT INTO texts (mode_id, title, content)
SELECT id, title, content FROM (
  SELECT id, 'Тестовый текст 1 для режима На время' AS title, 'Это первый тестовый текст для режима На время.' AS content FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 2 для режима На время', 'Второй текст для тренировки на время.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 3 для режима На время', 'Третий текст для повышения скорости набора.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 4 для режима На время', 'Четвёртый тренировочный текст для режима на время.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 5 для режима На время', 'Пятый текст для улучшения навыков набора.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 6 для режима На время', 'Шестой тестовый текст для развития скорости.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 7 для режима На время', 'Седьмой текст для режима тренировки на время.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 8 для режима На время', 'Восьмой текст для улучшения техники набора.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 9 для режима На время', 'Девятый тренировочный текст.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 10 для режима На время', 'Десятый текст для режима тренировки скорости.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 11 для режима На время', 'Одиннадцатый текст для тренировки на время.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 12 для режима На время', 'Двенадцатый тест для повышения скорости набора.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 13 для режима На время', 'Тринадцатый тренировочный текст для набора.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 14 для режима На время', 'Четырнадцатый текст для тренировки на время.' FROM modes WHERE key = 'time'
  UNION ALL
  SELECT id, 'Тестовый текст 15 для режима На время', 'Пятнадцатый тест для улучшения навыков набора.' FROM modes WHERE key = 'time'
) AS t
WHERE NOT EXISTS (
  SELECT 1 FROM texts WHERE mode_id = t.id AND title = t.title
);

-- Для режима 'quotes'
-- 12 цитат категории short
INSERT INTO quotes (content, length_cat, author) SELECT 'Жизнь — это путь, а не пункт назначения.', 'short', 'Будда' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Жизнь — это путь, а не пункт назначения.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Верь в себя.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Верь в себя.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Каждый день — шанс начать заново.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Каждый день — шанс начать заново.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Сделай первый шаг.', 'short', 'Мартин Лютер Кинг' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Сделай первый шаг.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Возможности не ждут.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Возможности не ждут.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Твори добро.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Твори добро.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Не бойся ошибаться.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Не бойся ошибаться.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Сила в простоте.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Сила в простоте.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Учись и развивайся.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Учись и развивайся.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Будь собой.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Будь собой.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Не сдавайся.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Не сдавайся.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Время — ценный ресурс.', 'short', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Время — ценный ресурс.') LIMIT 1;

-- 12 цитат категории medium
INSERT INTO quotes (content, length_cat, author) SELECT 'Неважно, как медленно ты идёшь, главное — не останавливаться.', 'medium', 'Конфуций' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Неважно, как медленно ты идёшь, главное — не останавливаться.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Учись, как будто будешь жить вечно, живи, как будто умрёшь сегодня.', 'medium', 'Махатма Ганди' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Учись, как будто будешь жить вечно, живи, как будто умрёшь сегодня.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Величайшая слава — не в том, чтобы никогда не падать, а в том, чтобы подниматься каждый раз.', 'medium', 'Конфуций' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Величайшая слава — не в том, чтобы никогда не падать, а в том, чтобы подниматься каждый раз.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Неудача — просто возможность начать снова, но уже более мудро.', 'medium', 'Генри Форд' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Неудача — просто возможность начать снова, но уже более мудро.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Твое время ограничено, не трать его, живя чужой жизнью.', 'medium', 'Стив Джобс' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Твое время ограничено, не трать его, живя чужой жизнью.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Секрет перемен — сосредоточить всю энергию не на борьбе со старым, а на создании нового.', 'medium', 'Сократ' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Секрет перемен — сосредоточить всю энергию не на борьбе со старым, а на создании нового.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Жизнь — это то, что с тобой происходит, пока ты строишь планы.', 'medium', 'Джон Леннон' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Жизнь — это то, что с тобой происходит, пока ты строишь планы.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Будь изменением, которое хочешь видеть в мире.', 'medium', 'Махатма Ганди' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Будь изменением, которое хочешь видеть в мире.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Действия — ключ к любому успеху.', 'medium', 'Пабло Пикассо' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Действия — ключ к любому успеху.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Верь, что можешь — и ты уже на полпути.', 'medium', 'Теодор Рузвельт' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Верь, что можешь — и ты уже на полпути.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Каждое утро — новая возможность изменить свою жизнь.', 'medium', 'Неизвестный автор' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Каждое утро — новая возможность изменить свою жизнь.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Если хочешь подняться — не строй лестницу на чужих спинах.', 'medium', 'Неизвестный автор' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Если хочешь подняться — не строй лестницу на чужих спинах.') LIMIT 1;

-- 12 цитат категории long
INSERT INTO quotes (content, length_cat, author) SELECT 'Жизнь коротка, искусство вечно, возможность мимолетна, опыт обманчив, решение трудно.', 'long', 'Гиппократ' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Жизнь коротка, искусство вечно, возможность мимолетна, опыт обманчив, решение трудно.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Человек не может открыть новые океаны, пока не имеет мужества потерять берег из виду.', 'long', 'Андре Жид' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Человек не может открыть новые океаны, пока не имеет мужества потерять берег из виду.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Истинное путешествие открытий не в поисках новых земель, а в новом взгляде на вещи.', 'long', 'Марсель Пруст' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Истинное путешествие открытий не в поисках новых земель, а в новом взгляде на вещи.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Сила человека не в том, что он может делать, а в том, что он способен преодолеть.', 'long', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Сила человека не в том, что он может делать, а в том, что он способен преодолеть.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Величайшее благо — это счастье, и каждый человек стремится к нему, даже если не знает, как его найти.', 'long', 'Аристотель' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Величайшее благо — это счастье, и каждый человек стремится к нему, даже если не знает, как его найти.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Будущее принадлежит тем, кто верит в красоту своих мечтаний.', 'long', 'Элеонор Рузвельт' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Будущее принадлежит тем, кто верит в красоту своих мечтаний.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Величие человека определяется не его успехами, а способностью подниматься после поражений.', 'long', 'Неизвестный' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Величие человека определяется не его успехами, а способностью подниматься после поражений.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Творчество — это позволение себе ошибаться, а искусство — это знание, какие ошибки оставить.', 'long', 'Скотт Адамс' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Творчество — это позволение себе ошибаться, а искусство — это знание, какие ошибки оставить.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Мы не можем изменить направление ветра, но можем настроить паруса так, чтобы всегда достигать цели.', 'long', 'Джим Рон' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Мы не можем изменить направление ветра, но можем настроить паруса так, чтобы всегда достигать цели.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Успех — это не ключ к счастью. Счастье — это ключ к успеху. Если ты любишь то, что делаешь, ты будешь успешен.', 'long', 'Альберт Швейцер' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Успех — это не ключ к счастью. Счастье — это ключ к успеху. Если ты любишь то, что делаешь, ты будешь успешен.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Жизнь измеряется не количеством вдохов, а моментами, когда захватывает дух.', 'long', 'Майя Энджелоу' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Жизнь измеряется не количеством вдохов, а моментами, когда захватывает дух.') LIMIT 1;
INSERT INTO quotes (content, length_cat, author) SELECT 'Чем больше ты читаешь, тем больше знаешь. Чем больше ты знаешь, тем дальше идёшь.', 'long', 'Фрэнсис Бэкон' WHERE NOT EXISTS (SELECT 1 FROM quotes WHERE content = 'Чем больше ты читаешь, тем больше знаешь. Чем больше ты знаешь, тем дальше идёшь.') LIMIT 1;



-- Для режима 'code' (Пользовательский код)
INSERT INTO code_snippets (lang, title, content)
SELECT 'cpp', 'Пример цикла for на C++', $$
for (int i = 0; i < 5; ++i) {
    std::cout << i << std::endl;
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Пример цикла for на C++'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'python', 'Пример цикла for на Python', $$
for i in range(5):
    print(i)
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Пример цикла for на Python'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'javascript', 'Пример цикла for на JavaScript', $$
for (let i = 0; i < 5; i++) {
    console.log(i);
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Пример цикла for на JavaScript'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'java', 'Пример цикла for на Java', $$
for (int i = 0; i < 5; i++) {
    System.out.println(i);
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Пример цикла for на Java'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'cpp', 'Условный оператор if на C++', $$
int a = 10;
if (a > 5) {
    std::cout << "a больше 5" << std::endl;
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Условный оператор if на C++'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'python', 'Условный оператор if на Python', $$
a = 10
if a > 5:
    print("a больше 5")
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Условный оператор if на Python'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'javascript', 'Условный оператор if на JavaScript', $$
let a = 10;
if (a > 5) {
    console.log("a больше 5");
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Условный оператор if на JavaScript'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'java', 'Условный оператор if на Java', $$
int a = 10;
if (a > 5) {
    System.out.println("a больше 5");
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Условный оператор if на Java'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'cpp', 'Функция на C++', $$
int sum(int a, int b) {
    return a + b;
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Функция на C++'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'python', 'Функция на Python', $$
def sum(a, b):
    return a + b
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Функция на Python'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'javascript', 'Функция на JavaScript', $$
function sum(a, b) {
    return a + b;
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Функция на JavaScript'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'java', 'Функция на Java', $$
int sum(int a, int b) {
    return a + b;
}
$$
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Функция на Java'
)
LIMIT 1;


-- Для режима 'words'
INSERT INTO words (word) VALUES
  ('альфа'),
  ('бета'),
  ('гамма'),
  ('дельта'),
  ('эпсилон'),
  ('зета'),
  ('эта'),
  ('тета'),
  ('йота'),
  ('каппа'),
  ('ламбда'),
  ('мю'),
  ('ню'),
  ('кси'),
  ('омикрон'),
  ('пи'),
  ('ро'),
  ('сигма'),
  ('тау'),
  ('упсилон'),
  ('фи'),
  ('хи'),
  ('пси'),
  ('омега'),
  ('слово1'),
  ('слово2'),
  ('слово3'),
  ('слово4'),
  ('слово5'),
  ('слово6'),
  ('слово7'),
  ('слово8'),
  ('слово9'),
  ('слово10'),
  ('слово11'),
  ('слово12'),
  ('слово13'),
  ('слово14'),
  ('слово15'),
  ('слово16');


-- ------------------------------------------------------------
-- 14) Владельцы и права
-- ------------------------------------------------------------

-- Владелец схемы и прав на БД
ALTER SCHEMA public OWNER TO textuser;
GRANT ALL PRIVILEGES ON DATABASE textdb TO textuser;

-- Сначала передаём таблицы
ALTER TABLE modes                  OWNER TO textuser;
ALTER TABLE texts                  OWNER TO textuser;
ALTER TABLE words                  OWNER TO textuser;
ALTER TABLE quotes                 OWNER TO textuser;
ALTER TABLE code_snippets          OWNER TO textuser;
ALTER TABLE players                OWNER TO textuser;
ALTER TABLE games                  OWNER TO textuser;
ALTER TABLE player_cumulative_stats OWNER TO textuser;

-- Затем указываем, какие последовательности принадлежат каким колонкам
-- Это нужно, чтобы избежать ошибок при смене владельца
-- и позволяет PostgreSQL автоматически менять владельца SEQUENCE при ALTER TABLE

-- Привязка SEQUENCE к колонкам (если они ещё не привязаны)
ALTER SEQUENCE IF EXISTS texts_id_seq   OWNED BY texts.id;
ALTER SEQUENCE IF EXISTS players_id_seq OWNED BY players.id;
ALTER SEQUENCE IF EXISTS games_id_seq   OWNED BY games.id;

-- Владельцы материализованных представлений
ALTER MATERIALIZED VIEW leaderboard_60 OWNER TO textuser;
ALTER MATERIALIZED VIEW leaderboard_15 OWNER TO textuser;

-- Владельцы функций
ALTER FUNCTION prune_old_games()         OWNER TO textuser;
ALTER FUNCTION update_cumulative_stats() OWNER TO textuser;
ALTER FUNCTION refresh_leaderboards()    OWNER TO textuser;
