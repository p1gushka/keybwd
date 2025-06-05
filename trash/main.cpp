#include <iostream>
#include <pqxx/pqxx>
#include "text_database.hpp"

int main()
{
    try
    {
        server::TextDatabase db("localhost", "textdb", "textuser", "secure_password", "5432");
        std::string username = "ivan_typist";
        int player_id = db.get_or_create_player(username);

        // Очищаем предыдущие данные для плеера
        {
            pqxx::connection conn("host=localhost dbname=textdb user=textuser password=secure_password port=5432");
            pqxx::work tx(conn);
            tx.exec_params("DELETE FROM games WHERE player_id = $1", player_id);
            tx.exec_params("DELETE FROM player_cumulative_stats WHERE player_id = $1", player_id);
            tx.commit();
        }

        db.record_game(username, 58.75, 60.00, 97.20, 450, 10, 2, 1);
        db.record_game(username, 62.30, 64.00, 98.50, 470, 8, 1, 0);
        db.record_game(username, 60.10, 61.50, 96.80, 460, 12, 3, 1);
        db.record_game(username, 65.00, 66.20, 99.00, 480, 5, 0, 0);
        db.record_game(username, 63.40, 64.80, 98.20, 475, 7, 2, 0);
        db.record_game(username, 0, 0, 0, 0, 3, 0, 0);

        pqxx::connection C("host=localhost dbname=textdb user=textuser password=secure_password port=5432");
        pqxx::nontransaction tx(C);

        auto res_games = tx.exec_params(
            "SELECT played_at, speed_wpm, raw_wpm, accuracy, correct_symbols, wrong_symbols, missed_symbols, extra_symbols "
            "FROM games WHERE player_id = $1 ORDER BY played_at DESC LIMIT 5",
            player_id);

        std::cout << "=== Last 5 games for " << username << " ===\n";
        for (auto row : res_games)
        {
            std::cout << row["played_at"].as<std::string>() << " | "
                      << row["speed_wpm"].as<double>() << " WPM | "
                      << row["raw_wpm"].as<double>() << " RAW | "
                      << row["accuracy"].as<double>() << "% accuracy | "
                      << "correct: " << row["correct_symbols"].as<int>() << ", "
                      << "wrong: " << row["wrong_symbols"].as<int>() << ", "
                      << "missed: " << row["missed_symbols"].as<int>() << ", "
                      << "extra: " << row["extra_symbols"].as<int>() << "\n";
        }
        std::cout << std::endl;

        auto res_stats = tx.exec_params(
            "SELECT total_games, sum_speed_wpm, sum_raw_wpm, sum_accuracy, "
            "sum_correct_symbols, sum_wrong_symbols, sum_missed_symbols, sum_extra_symbols "
            "FROM player_cumulative_stats WHERE player_id = $1",
            player_id);

        if (!res_stats.empty())
        {
            auto row = res_stats[0];
            long total = row["total_games"].as<long>();
            double sum_sp = row["sum_speed_wpm"].as<double>();
            double sum_raw = row["sum_raw_wpm"].as<double>();
            double sum_acc = row["sum_accuracy"].as<double>();
            long sum_corr = row["sum_correct_symbols"].as<long>();
            long sum_wrong = row["sum_wrong_symbols"].as<long>();
            long sum_missed = row["sum_missed_symbols"].as<long>();
            long sum_extra = row["sum_extra_symbols"].as<long>();

            std::cout << "=== Cumulative stats for " << username << " ===\n"
                      << "Total games: " << total << "\n"
                      << "Sum speed WPM: " << sum_sp << "\n"
                      << "Sum raw WPM: " << sum_raw << "\n"
                      << "Sum accuracy: " << sum_acc << "\n"
                      << "Sum correct: " << sum_corr << "\n"
                      << "Sum wrong: " << sum_wrong << "\n"
                      << "Sum missed: " << sum_missed << "\n"
                      << "Sum extra: " << sum_extra << "\n\n";

            std::cout << "=== Average stats (computed in code) ===\n"
                      << "Avg speed WPM: " << (sum_sp / total) << "\n"
                      << "Avg raw WPM: " << (sum_raw / total) << "\n"
                      << "Avg accuracy: " << (sum_acc / total) << "\n"
                      << "Avg correct: " << (double(sum_corr) / total) << "\n"
                      << "Avg wrong: " << (double(sum_wrong) / total) << "\n"
                      << "Avg missed: " << (double(sum_missed) / total) << "\n"
                      << "Avg extra: " << (double(sum_extra) / total) << "\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
