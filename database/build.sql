-- Давайте вы скачали как-то SQL и пытаетесь запустить этот скрипт.
-- Такая команда для запуска: psql -U <имя юзера> -d postgres -f build.sql
-- 1. Ошибка: Peer authentication failed for user
-- * sudo nano /etc/postgresql/<версия>/main/pg_hba.conf
-- * Найдите строку: local   all   all   peer
-- и замените peer на scram-sha-256
-- * Перезапустите PostgreSQL: sudo systemctl restart postgresql
-- * psql -U <имя юзера> -d postgres -f build.sql
-- Ну юзер это чел, которого поможет создать гпт + пароль.

-- Создает базу данных, таблицу texts и добавляет тестовые данные.

-- 1. Проверяем существование базы данных и создаем ее.
SELECT 'CREATE DATABASE textdb'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'textdb')\gexec

-- 2. Подключаемся к созданной базе
\c textdb

-- 3. Создаем таблицу texts1 (с проверкой на существование)
-- DROP TABLE IF EXISTS texts; 
CREATE TABLE IF NOT EXISTS texts (
    id SERIAL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);

-- 4. Очищаем таблицу перед добавлением новых данных
TRUNCATE TABLE texts RESTART IDENTITY;

-- 5. Добавляем тестовые данные
INSERT INTO texts (title, content) VALUES
('Первый текст', 'Это содержимое первого текста в нашей базе данных'),
('Второй текст', 'Здесь находится содержание второго текстового документа');

-- 6. Проверяем, что данные добавились
SELECT * FROM texts;

-- 7. Создаем таблицу players (если нет)
CREATE TABLE IF NOT EXISTS players (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) NOT NULL
);

-- 8. Дополняем players колонками авторизации (если их нет)
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='players' AND column_name='login') THEN
        ALTER TABLE players ADD COLUMN login VARCHAR(50) NOT NULL DEFAULT 'temp_login';
        UPDATE players SET login = username;
        ALTER TABLE players ALTER COLUMN login DROP DEFAULT;
        ALTER TABLE players ADD CONSTRAINT login_unique UNIQUE (login);
    END IF;

    IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='players' AND column_name='password_hash') THEN
        ALTER TABLE players ADD COLUMN password_hash TEXT NOT NULL DEFAULT '';
    END IF;

    IF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='players' AND column_name='registered_at') THEN
        ALTER TABLE players ADD COLUMN registered_at TIMESTAMP DEFAULT NOW();
    END IF;
END
$$;

-- 9. Создаем таблицу games
CREATE TABLE IF NOT EXISTS games (
    id SERIAL PRIMARY KEY,
    player_id INTEGER NOT NULL REFERENCES players(id) ON DELETE CASCADE,
    played_at TIMESTAMP NOT NULL DEFAULT NOW(),
    speed_wpm NUMERIC(5,2) NOT NULL,
    raw_wpm NUMERIC(5,2) NOT NULL,
    accuracy NUMERIC(5,2) NOT NULL,
    correct_symbols INTEGER NOT NULL,
    wrong_symbols INTEGER NOT NULL,
    missed_symbols INTEGER NOT NULL,
    extra_symbols INTEGER NOT NULL
);

-- 10. Функция: ограничиваем таблицу games до 5 последних записей на игрока
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

-- 11. Триггер на games
CREATE TRIGGER trg_prune_games
AFTER INSERT ON games
FOR EACH ROW
EXECUTE FUNCTION prune_old_games();

-- 12. Таблица накопленной статистики
CREATE TABLE IF NOT EXISTS player_cumulative_stats (
    player_id INTEGER PRIMARY KEY REFERENCES players(id) ON DELETE CASCADE,
    total_games BIGINT NOT NULL DEFAULT 0,
    sum_speed_wpm NUMERIC(20,4) NOT NULL DEFAULT 0,
    sum_raw_wpm NUMERIC(20,4) NOT NULL DEFAULT 0,
    sum_accuracy NUMERIC(20,4) NOT NULL DEFAULT 0,
    sum_correct_symbols BIGINT NOT NULL DEFAULT 0,
    sum_wrong_symbols BIGINT NOT NULL DEFAULT 0,
    sum_missed_symbols BIGINT NOT NULL DEFAULT 0,
    sum_extra_symbols BIGINT NOT NULL DEFAULT 0
);

-- 13. Функция для обновления статистики
CREATE OR REPLACE FUNCTION update_cumulative_stats()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO player_cumulative_stats(
        player_id,
        total_games,
        sum_speed_wpm,
        sum_raw_wpm,
        sum_accuracy,
        sum_correct_symbols,
        sum_wrong_symbols,
        sum_missed_symbols,
        sum_extra_symbols
    )
    VALUES (
        NEW.player_id,
        1,
        NEW.speed_wpm,
        NEW.raw_wpm,
        NEW.accuracy,
        NEW.correct_symbols,
        NEW.wrong_symbols,
        NEW.missed_symbols,
        NEW.extra_symbols
    )
    ON CONFLICT (player_id) DO
      UPDATE SET
        total_games = player_cumulative_stats.total_games + 1,
        sum_speed_wpm = player_cumulative_stats.sum_speed_wpm + NEW.speed_wpm,
        sum_raw_wpm = player_cumulative_stats.sum_raw_wpm + NEW.raw_wpm,
        sum_accuracy = player_cumulative_stats.sum_accuracy + NEW.accuracy,
        sum_correct_symbols = player_cumulative_stats.sum_correct_symbols + NEW.correct_symbols,
        sum_wrong_symbols = player_cumulative_stats.sum_wrong_symbols + NEW.wrong_symbols,
        sum_missed_symbols = player_cumulative_stats.sum_missed_symbols + NEW.missed_symbols,
        sum_extra_symbols = player_cumulative_stats.sum_extra_symbols + NEW.extra_symbols
    ;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

-- 14. Триггер на обновление статистики
CREATE TRIGGER trg_update_cumulative
AFTER INSERT ON games
FOR EACH ROW
EXECUTE FUNCTION update_cumulative_stats();
