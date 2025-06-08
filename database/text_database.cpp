#include "text_database.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include <functional>

namespace server
{
    namespace
    {
        // Простое хэширование пароля
        std::string simple_hash(const std::string &input)
        {
            std::hash<std::string> hasher;
            return std::to_string(hasher(input));
        }
    }

    TextDatabase::TextDatabase(const std::string &host,
                               const std::string &dbname,
                               const std::string &user,
                               const std::string &password,
                               const std::string &port)
        : DatabaseBase(
              "host=" + host + " " +
              "dbname=" + dbname + " " +
              "user=" + user + " " +
              "password=" + password + " " +
              "port=" + port)
    {
        std::cout << "Connected to text database successfully\n";
    }

    bool TextDatabase::register_player(const std::string &login, const std::string &username, const std::string &password)
    {
        verify_connection();
        std::string hash = simple_hash(password);
        pqxx::work tx(*conn_);
        auto res = tx.exec_params("SELECT id FROM players WHERE login = $1", login);
        if (!res.empty())
            return false;
        tx.exec_params(
            "INSERT INTO players (login, username, password_hash) VALUES ($1, $2, $3)",
            login, username, hash);
        tx.commit();
        return true;
    }

    std::optional<int> TextDatabase::authenticate_player(const std::string &login, const std::string &password)
    {
        verify_connection();
        std::string hash = simple_hash(password);
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT id FROM players WHERE login = $1 AND password_hash = $2",
            login, hash);
        if (res.empty())
            return std::nullopt;
        return res[0]["id"].as<int>();
    }

    std::string TextDatabase::get_username(int player_id) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params("SELECT username FROM players WHERE id = $1", player_id);
        if (res.empty())
            throw std::runtime_error("Player not found");
        return res[0][0].as<std::string>();
    }

    void TextDatabase::insert_text(const std::string &content, const std::string &title)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        try
        {
            tx.exec_params("INSERT INTO texts (title, content) VALUES ($1, $2)", title, content);
            tx.commit();
        }
        catch (const std::exception &e)
        {
            tx.abort();
            throw std::runtime_error("Insert failed: " + std::string(e.what()));
        }
    }

    std::string TextDatabase::get_random_text() const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        try
        {
            auto res = tx.exec1("SELECT content FROM texts ORDER BY RANDOM() LIMIT 1");
            return res[0].as<std::string>();
        }
        catch (const pqxx::unexpected_rows &)
        {
            return "";
        }
    }

    void TextDatabase::print_all() const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec("SELECT id, title, content, created_at FROM texts ORDER BY id");
        if (res.empty())
        {
            std::cout << "Empty table\n";
            return;
        }
        for (auto row : res)
        {
            std::cout << "ID: " << row["id"].as<int>() << "\n"
                      << "Title: " << row["title"].as<std::string>() << "\n"
                      << "Content: " << row["content"].as<std::string>() << "\n"
                      << "Created: " << row["created_at"].as<std::string>() << "\n"
                      << "----------------\n";
        }
    }

    void TextDatabase::delete_by_id(int id)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        auto res = tx.exec_params("DELETE FROM texts WHERE id = $1 RETURNING id", id);
        tx.commit();
        std::cout << (res.empty() ? "Nothing deleted\n" : "Deleted text ID " + std::to_string(id) + "\n");
    }

    void TextDatabase::delete_by_content(const std::string &content)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        auto res = tx.exec_params("DELETE FROM texts WHERE content = $1 RETURNING id", content);
        tx.commit();
        std::cout << "Deleted " << res.size() << " entries\n";
    }

    void TextDatabase::clear_table()
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec("TRUNCATE TABLE texts RESTART IDENTITY");
        tx.commit();
    }

    void TextDatabase::record_game(int player_id,
                                   const std::string &mode,
                                   double speed_wpm,
                                   double raw_wpm,
                                   double accuracy,
                                   int correct_symbols,
                                   int wrong_symbols,
                                   int missed_symbols,
                                   int extra_symbols)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec_params(
            "INSERT INTO games(player_id, mode, speed_wpm, raw_wpm, accuracy, correct_symbols, wrong_symbols, missed_symbols, extra_symbols) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
            player_id, mode, speed_wpm, raw_wpm, accuracy,
            correct_symbols, wrong_symbols, missed_symbols, extra_symbols);
        tx.commit();
    }

    AverageStats TextDatabase::get_average_stats(int player_id) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT sum_speed_wpm, sum_raw_wpm, sum_accuracy, sum_correct_symbols, sum_wrong_symbols, sum_missed_symbols, sum_extra_symbols, total_games "
            "FROM player_cumulative_stats WHERE player_id = $1",
            player_id);
        if (res.empty())
            throw std::runtime_error("No statistics for player");

        double sum_speed = res[0]["sum_speed_wpm"].as<double>();
        double sum_raw = res[0]["sum_raw_wpm"].as<double>();
        double sum_acc = res[0]["sum_accuracy"].as<double>();
        double sum_corr = res[0]["sum_correct_symbols"].as<double>();
        double sum_wrong = res[0]["sum_wrong_symbols"].as<double>();
        double sum_missed = res[0]["sum_missed_symbols"].as<double>();
        double sum_extra = res[0]["sum_extra_symbols"].as<double>();
        double total = res[0]["total_games"].as<double>();

        AverageStats stats;
        stats.avg_speed_wpm = sum_speed / total;
        stats.avg_raw_wpm = sum_raw / total;
        stats.avg_accuracy = sum_acc / total;
        stats.avg_correct_symbols = sum_corr / total;
        stats.avg_wrong_symbols = sum_wrong / total;
        stats.avg_missed_symbols = sum_missed / total;
        stats.avg_extra_symbols = sum_extra / total;
        return stats;
    }

    std::vector<LeaderboardEntry> TextDatabase::get_leaderboard(const std::string &mode, int limit) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT username, speed_wpm, accuracy, played_at "
            "FROM leaderboard_" +
                mode + " "
                       "ORDER BY speed_wpm DESC "
                       "LIMIT $1",
            limit);

        std::vector<LeaderboardEntry> board;
        for (auto row : res)
        {
            board.push_back({row["username"].as<std::string>(),
                             row["speed_wpm"].as<double>(),
                             row["accuracy"].as<double>(),
                             row["played_at"].as<std::string>()});
        }
        return board;
    }

    void TextDatabase::refresh_leaderboards()
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec("REFRESH MATERIALIZED VIEW leaderboard_60");
        tx.exec("REFRESH MATERIALIZED VIEW leaderboard_15");
        tx.commit();
    }

} // namespace server
