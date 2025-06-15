#include <iostream>
#include <pqxx/pqxx>
#include "text_database.hpp"

void insert_sample_content(server::TextDatabase &db)
{
    // Для режима "На время" (id = 1)
    db.insert_text(1, "Тест 1: На время", "Первый тестовый текст для режима На время.");
    db.insert_text(1, "Тест 2: На время", "Второй текст. Сложнее. Быстрее.");

    // Для режима "По словам" (id = 2)
    db.insert_word("альфа");
    db.insert_word("бета");
    db.insert_word("гамма");
    db.insert_word("дельта");
    db.insert_word("эпсилон");

    // Для режима "Цитаты"
    db.insert_quote("Жизнь — это то, что с тобой происходит, пока ты строишь планы.", "medium", "Джон Леннон");
    db.insert_quote("Всё проходит, и это тоже пройдёт.", "short", "Народная мудрость");

    // Для режима "Код"
    db.insert_code_snippet("cpp", "Hello World C++", "std::cout << \"Привет, мир!\" << std::endl;");
    db.insert_code_snippet("python", "Hello World Python", "print(\"Привет, мир!\")");
}

void show_random_content(server::TextDatabase &db)
{
    std::cout << "\n-- Случайный текст (На время):\n"
              << db.get_random_text(1) << "\n";

    std::cout << "\n-- Случайные слова (По словам):\n";
    for (const auto &w : db.get_random_words(5))
        std::cout << w << " ";
    std::cout << "\n";

    std::cout << "-- Случайная цитата (medium):\n"
              << db.get_random_quote("medium") << "\n";

    std::cout << "-- Случайный код (cpp):\n"
              << db.get_random_code("cpp") << "\n";
}

int main()
{
    try
    {
        server::TextDatabase db("localhost", "textdb", "textuser", "secure_password", "5432");

        // Авторизация
        const std::string login = "test_user";
        const std::string username = "Tester";
        const std::string password = "pass123";

        if (!db.register_player(login, username, password))
        {
            std::cout << "[Info] Пользователь уже существует, пробуем аутентифицировать...\n";
        }

        auto pid = db.authenticate_player(login, password);
        if (!pid)
        {
            std::cerr << "Ошибка аутентификации!\n";
            return 1;
        }

        int player_id = *pid;
        std::cout << "[OK] Игрок ID=" << player_id << " вошёл как '" << username << "'\n";

        // Заполняем БД контентом
        insert_sample_content(db);

        // Показываем случайный контент
        show_random_content(db);

        // Запись пары игр
        db.record_game(player_id, "60", 71.2, 73.0, 96.1, 360, 11, 5, 1);
        db.record_game(player_id, "60", 89.96, 79.0, 98.0, 400, 6, 2, 0);

        db.refresh_leaderboards();

        // Статистика
        auto stats = db.get_average_stats(player_id);
        std::cout << "\n=== Средняя статистика ===\n"
                  << "WPM: " << stats.avg_speed_wpm
                  << ", Raw WPM: " << stats.avg_raw_wpm
                  << ", Accuracy: " << stats.avg_accuracy << "%\n"
                  << "Правильных: " << stats.avg_correct_symbols
                  << ", Ошибок: " << stats.avg_wrong_symbols << "\n";

        // Лидерборд
        std::cout << "\n=== Лидерборд (60 сек) ===\n";
        for (auto &e : db.get_leaderboard("60"))
        {
            std::cout << e.username << " | "
                      << e.speed_wpm << " WPM | "
                      << e.accuracy << "% | "
                      << e.played_at << "\n";
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }
}
