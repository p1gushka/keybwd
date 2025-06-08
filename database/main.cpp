#include <iostream>
#include <pqxx/pqxx>
#include "text_database.hpp"

int main()
{
    try
    {
        server::TextDatabase db("localhost", "textdb", "textuser", "secure_password", "5432");

        std::string login, password, username;
        int choice;

        std::cout << "\nВыберите действие:\n[1] Войти\n[2] Зарегистрироваться\n> ";
        std::cin >> choice;

        std::optional<int> player_id;

        if (choice == 1)
        {
            std::cout << "Login: ";
            std::cin >> login;
            std::cout << "Password: ";
            std::cin >> password;

            player_id = db.authenticate_player(login, password);
            if (!player_id)
            {
                std::cerr << "Неверный логин или пароль\n";
                return 1;
            }
            username = db.get_username(*player_id);
        }
        else if (choice == 2)
        {
            std::cout << "Придумайте login: ";
            std::cin >> login;
            std::cout << "Придумайте пароль: ";
            std::cin >> password;
            std::cout << "Ваше имя (username): ";
            std::cin >> username;

            if (!db.register_player(login, username, password))
            {
                std::cerr << "Регистрация не удалась (логин уже занят)\n";
                return 1;
            }

            player_id = db.authenticate_player(login, password);
            if (!player_id)
            {
                std::cerr << "Ошибка после регистрации\n";
                return 1;
            }
        }
        else
        {
            std::cerr << "Некорректный выбор\n";
            return 1;
        }
        db.refresh_leaderboards();

        // Тестовые записи игр
        db.record_game(*player_id, "60", 58.75, 60.00, 97.20, 450, 10, 2, 1);
        db.record_game(*player_id, "60", 62.30, 64.00, 98.50, 470, 8, 1, 0);
        db.record_game(*player_id, "15", 60.10, 61.50, 96.80, 460, 12, 3, 1);
        db.record_game(*player_id, "15", 65.00, 66.20, 99.00, 480, 5, 0, 0);
        db.refresh_leaderboards();

        auto top_60 = db.get_leaderboard("60", 10);
        std::cout << "\n=== Leaderboard (60 сек) ===\n";
        for (const auto &entry : top_60)
        {
            std::cout << entry.username << " | "
                      << entry.speed_wpm << " WPM | "
                      << entry.accuracy << "% | "
                      << entry.played_at << "\n";
        }

        auto top_15 = db.get_leaderboard("15", 10);
        std::cout << "\n=== Leaderboard (15 сек) ===\n";
        for (const auto &entry : top_15)
        {
            std::cout << entry.username << " | "
                      << entry.speed_wpm << " WPM | "
                      << entry.accuracy << "% | "
                      << entry.played_at << "\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
