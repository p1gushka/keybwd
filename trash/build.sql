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
-- TRUNCATE TABLE texts RESTART IDENTITY;

-- 5. Добавляем тестовые данные
INSERT INTO texts (title, content) VALUES
('Первый текст', 'Это содержимое первого текста в нашей базе данных'),
('Второй текст', 'Здесь находится содержание второго текстового документа');

-- 6. Проверяем, что данные добавились
SELECT * FROM texts;


