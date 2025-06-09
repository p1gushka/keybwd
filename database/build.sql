-- =========================
-- Шаг 0: Создание пользователя и базы
-- =========================

DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_roles WHERE rolname = 'textuser') THEN
        CREATE USER textuser WITH PASSWORD 'secure_password';
    END IF;
END
$$;

SELECT 'CREATE DATABASE textdb WITH OWNER textuser'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'textdb')\gexec

\c textdb

-- =========================
-- Шаг 1: Передаём владельца схемы и базы
-- =========================
ALTER SCHEMA public OWNER TO textuser;
GRANT ALL PRIVILEGES ON DATABASE textdb TO textuser;

-- =========================
-- Шаг 2: Таблица texts
-- =========================
CREATE TABLE IF NOT EXISTS texts (
    id SERIAL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);

INSERT INTO texts (title, content) VALUES
('Первый текст', 'Это содержимое первого текста'),
('Второй текст', 'Это другой текстовый фрагмент');

-- =========================
-- Шаг 3: Таблица players с авторизацией
-- =========================
CREATE TABLE IF NOT EXISTS players (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    login VARCHAR(50) NOT NULL UNIQUE,
    password_hash TEXT NOT NULL DEFAULT '',
    registered_at TIMESTAMP DEFAULT NOW()
);

-- =========================
-- Шаг 4: Таблица games
-- =========================
CREATE TABLE IF NOT EXISTS games (
    id SERIAL PRIMARY KEY,
    player_id INTEGER NOT NULL REFERENCES players(id) ON DELETE CASCADE,
    played_at TIMESTAMP NOT NULL DEFAULT NOW(),
    mode VARCHAR(10) NOT NULL DEFAULT '60',
    speed_wpm NUMERIC(5,2) NOT NULL,
    raw_wpm NUMERIC(5,2) NOT NULL,
    accuracy NUMERIC(5,2) NOT NULL,
    correct_symbols INTEGER NOT NULL,
    wrong_symbols INTEGER NOT NULL,
    missed_symbols INTEGER NOT NULL,
    extra_symbols INTEGER NOT NULL
);

-- =========================
-- Шаг 5: Присваиваем владельца sequence
-- =========================
ALTER SEQUENCE texts_id_seq OWNER TO textuser;
ALTER SEQUENCE players_id_seq OWNER TO textuser;
ALTER SEQUENCE games_id_seq OWNER TO textuser;
-- (Добавьте другие sequence, если они созданы автоматически)

-- =========================
-- Шаг 6: Триггер prune_old_games
-- =========================
DROP TRIGGER IF EXISTS trg_prune_games ON games;

CREATE OR REPLACE FUNCTION prune_old_games()
RETURNS TRIGGER AS $$
BEGIN
    DELETE FROM games
    WHERE id IN (
        SELECT id FROM games
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

-- =========================
-- Шаг 7: Таблица player_cumulative_stats
-- =========================
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

-- =========================
-- Шаг 8: Триггер обновления статистики
-- =========================
DROP TRIGGER IF EXISTS trg_update_cumulative ON games;

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
        NEW.player_id, 1, NEW.speed_wpm, NEW.raw_wpm, NEW.accuracy,
        NEW.correct_symbols, NEW.wrong_symbols, NEW.missed_symbols, NEW.extra_symbols
    )
    ON CONFLICT (player_id) DO UPDATE
    SET
        total_games = player_cumulative_stats.total_games + 1,
        sum_speed_wpm = player_cumulative_stats.sum_speed_wpm + NEW.speed_wpm,
        sum_raw_wpm = player_cumulative_stats.sum_raw_wpm + NEW.raw_wpm,
        sum_accuracy = player_cumulative_stats.sum_accuracy + NEW.accuracy,
        sum_correct_symbols = player_cumulative_stats.sum_correct_symbols + NEW.correct_symbols,
        sum_wrong_symbols = player_cumulative_stats.sum_wrong_symbols + NEW.wrong_symbols,
        sum_missed_symbols = player_cumulative_stats.sum_missed_symbols + NEW.missed_symbols,
        sum_extra_symbols = player_cumulative_stats.sum_extra_symbols + NEW.extra_symbols;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_update_cumulative
AFTER INSERT ON games
FOR EACH ROW
EXECUTE FUNCTION update_cumulative_stats();

-- =========================
-- Шаг 9: Материализованные представления лидеров (уникальные пользователи)
-- =========================
CREATE MATERIALIZED VIEW leaderboard_60 AS
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

CREATE MATERIALIZED VIEW leaderboard_15 AS
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

-- =========================
-- Шаг 10: Функция обновления лидербордов
-- =========================
CREATE OR REPLACE FUNCTION refresh_leaderboards()
RETURNS VOID AS $$
BEGIN
    REFRESH MATERIALIZED VIEW leaderboard_60;
    REFRESH MATERIALIZED VIEW leaderboard_15;
END;
$$ LANGUAGE plpgsql;

-- =========================
-- Шаг 11: Передаём владельца объектов
-- =========================
ALTER TABLE texts OWNER TO textuser;
ALTER TABLE players OWNER TO textuser;
ALTER TABLE games OWNER TO textuser;
ALTER TABLE player_cumulative_stats OWNER TO textuser;

ALTER MATERIALIZED VIEW leaderboard_60 OWNER TO textuser;
ALTER MATERIALIZED VIEW leaderboard_15 OWNER TO textuser;

ALTER FUNCTION prune_old_games() OWNER TO textuser;
ALTER FUNCTION update_cumulative_stats() OWNER TO textuser;
ALTER FUNCTION refresh_leaderboards() OWNER TO textuser;