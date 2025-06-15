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
CREATE TABLE IF NOT EXISTS words (
    id   SERIAL PRIMARY KEY,
    word VARCHAR(100) UNIQUE NOT NULL
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
-- Для режима 'time' (На время) - 2 текста
INSERT INTO texts (mode_id, title, content)
SELECT id, 'Тестовый текст 1 для режима На время', 'Это первый тестовый текст для режима На время.'
FROM modes WHERE key = 'time'
AND NOT EXISTS (
    SELECT 1 FROM texts WHERE mode_id = modes.id AND title = 'Тестовый текст 1 для режима На время'
)
LIMIT 1;

INSERT INTO texts (mode_id, title, content)
SELECT id, 'Тестовый текст 2 для режима На время', 'Второй текст для тренировки на время.'
FROM modes WHERE key = 'time'
AND NOT EXISTS (
    SELECT 1 FROM texts WHERE mode_id = modes.id AND title = 'Тестовый текст 2 для режима На время'
)
LIMIT 1;

-- Для режима 'words' (По словам) - 2 текста
INSERT INTO texts (mode_id, title, content)
SELECT id, 'Тестовый текст 1 для режима По словам', 'алфа бета гамма дельта эпсилон'
FROM modes WHERE key = 'words'
AND NOT EXISTS (
    SELECT 1 FROM texts WHERE mode_id = modes.id AND title = 'Тестовый текст 1 для режима По словам'
)
LIMIT 1;

INSERT INTO texts (mode_id, title, content)
SELECT id, 'Тестовый текст 2 для режима По словам', 'зета эта тета йота каппа'
FROM modes WHERE key = 'words'
AND NOT EXISTS (
    SELECT 1 FROM texts WHERE mode_id = modes.id AND title = 'Тестовый текст 2 для режима По словам'
)
LIMIT 1;

-- Для режима 'quotes' (Цитаты) - 2 цитаты
INSERT INTO quotes (content, length_cat, author)
SELECT 'Жизнь — это то, что с тобой происходит, пока ты строишь планы.', 'medium', 'Джон Леннон'
WHERE NOT EXISTS (
    SELECT 1 FROM quotes WHERE content = 'Жизнь — это то, что с тобой происходит, пока ты строишь планы.'
)
LIMIT 1;

INSERT INTO quotes (content, length_cat, author)
SELECT 'Всё проходит, и это тоже пройдёт.', 'short', 'Персидская пословица'
WHERE NOT EXISTS (
    SELECT 1 FROM quotes WHERE content = 'Всё проходит, и это тоже пройдёт.'
)
LIMIT 1;

-- Для режима 'code' (Пользовательский код) - 2 примера кода
INSERT INTO code_snippets (lang, title, content)
SELECT 'cpp', 'Пример Hello World на C++', 'std::cout << "Привет, мир!" << std::endl;'
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Пример Hello World на C++'
)
LIMIT 1;

INSERT INTO code_snippets (lang, title, content)
SELECT 'python', 'Пример Hello World на Python', 'print("Привет, мир!")'
WHERE NOT EXISTS (
    SELECT 1 FROM code_snippets WHERE title = 'Пример Hello World на Python'
)
LIMIT 1;
-- ------------------------------------------------------------
-- 14) Владельцы и права
-- ------------------------------------------------------------
ALTER SCHEMA public                OWNER TO textuser;
GRANT ALL PRIVILEGES ON DATABASE textdb TO textuser;

ALTER TABLE texts                  OWNER TO textuser;
ALTER TABLE players                OWNER TO textuser;
ALTER TABLE games                  OWNER TO textuser;
ALTER TABLE player_cumulative_stats OWNER TO textuser;
ALTER TABLE modes                  OWNER TO textuser;
ALTER TABLE words                  OWNER TO textuser;
ALTER TABLE quotes                 OWNER TO textuser;
ALTER TABLE code_snippets          OWNER TO textuser;

ALTER MATERIALIZED VIEW leaderboard_60 OWNER TO textuser;
ALTER MATERIALIZED VIEW leaderboard_15 OWNER TO textuser;

ALTER FUNCTION prune_old_games()       OWNER TO textuser;
ALTER FUNCTION update_cumulative_stats() OWNER TO textuser;
ALTER FUNCTION refresh_leaderboards()    OWNER TO textuser;
