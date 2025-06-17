#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "text_database.hpp"
#include <pqxx/pqxx>
#include <optional>
#include <stdexcept>
#include <functional>
#include <iostream>

namespace server
{
    namespace
    {
        std::string simple_hash(const std::string &input)
        {
            std::hash<std::string> hasher;
            return std::to_string(hasher(input));
        }
    } // namespace

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

    bool TextDatabase::register_player(const std::string &login,
                                       const std::string &username,
                                       const std::string &password)
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

    std::optional<int> TextDatabase::authenticate_player(const std::string &login,
                                                         const std::string &password)
    {
        verify_connection();
        std::string hash = simple_hash(password);
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT id FROM players WHERE login = $1 AND password_hash = $2",
            login, hash);
        if (res.empty())
            return std::nullopt;
        return res[0][0].as<int>();
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

    void TextDatabase::insert_text(int mode_id,
                                   const std::string &title,
                                   const std::string &content)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec_params(
            "INSERT INTO texts (mode_id, title, content) VALUES ($1, $2, $3)",
            mode_id, title, content);
        tx.commit();
    }

    std::string TextDatabase::get_random_text(int mode_id) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT content FROM texts WHERE mode_id = $1 ORDER BY RANDOM() LIMIT 1",
            mode_id);
        return res.empty() ? std::string() : res[0][0].as<std::string>();
    }

    void TextDatabase::insert_word(const std::string &word)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec_params("INSERT INTO words (word) VALUES ($1) ON CONFLICT DO NOTHING", word);
        tx.commit();
    }

    std::vector<std::string> TextDatabase::get_random_words(int count) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT word FROM words ORDER BY RANDOM() LIMIT $1", count);
        std::vector<std::string> list;
        for (auto row : res)
            list.push_back(row[0].as<std::string>());
        return list;
    }

    void TextDatabase::insert_quote(const std::string &content,
                                    const std::string &length_cat,
                                    const std::string &author)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec_params(
            "INSERT INTO quotes (content, length_cat, author) VALUES ($1, $2, $3)",
            content, length_cat, author);
        tx.commit();
    }

    std::string TextDatabase::get_random_quote(const std::string &length_cat) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT content FROM quotes WHERE length_cat = $1 ORDER BY RANDOM() LIMIT 1",
            length_cat);
        return res.empty() ? std::string() : res[0][0].as<std::string>();
    }

    void TextDatabase::insert_code_snippet(const std::string &lang,
                                           const std::string &title,
                                           const std::string &content)
    {
        verify_connection();
        pqxx::work tx(*conn_);
        tx.exec_params(
            "INSERT INTO code_snippets (lang, title, content) VALUES ($1, $2, $3)",
            lang, title, content);
        tx.commit();
    }

    std::string TextDatabase::get_random_code(const std::string &lang) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT content FROM code_snippets WHERE lang = $1 ORDER BY RANDOM() LIMIT 1",
            lang);
        return res.empty() ? std::string() : res[0][0].as<std::string>();
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
            "INSERT INTO games (player_id, mode, speed_wpm, raw_wpm, accuracy, correct_symbols, wrong_symbols, missed_symbols, extra_symbols)"
            " VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9)",
            player_id, mode, speed_wpm, raw_wpm, accuracy,
            correct_symbols, wrong_symbols, missed_symbols, extra_symbols);
        tx.commit();
    }

    AverageStats TextDatabase::get_average_stats(int player_id) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT sum_speed_wpm, sum_raw_wpm, sum_accuracy, sum_correct_symbols, sum_wrong_symbols, sum_missed_symbols, sum_extra_symbols, total_games"
            " FROM player_cumulative_stats WHERE player_id = $1",
            player_id);
        if (res.empty())
            throw std::runtime_error("No statistics for player");

        double sum_speed = res[0]["sum_speed_wpm"].as<double>();
        double sum_raw = res[0]["sum_raw_wpm"].as<double>();
        double sum_acc = res[0]["sum_accuracy"].as<double>();
        double total = res[0]["total_games"].as<double>();

        AverageStats stats;
        stats.avg_speed_wpm = sum_speed / total;
        stats.avg_raw_wpm = sum_raw / total;
        stats.avg_accuracy = sum_acc / total;
        stats.avg_correct_symbols = res[0]["sum_correct_symbols"].as<double>() / total;
        stats.avg_wrong_symbols = res[0]["sum_wrong_symbols"].as<double>() / total;
        stats.avg_missed_symbols = res[0]["sum_missed_symbols"].as<double>() / total;
        stats.avg_extra_symbols = res[0]["sum_extra_symbols"].as<double>() / total;
        return stats;
    }

    std::vector<LeaderboardEntry> TextDatabase::get_leaderboard(const std::string &mode) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        std::string view = "leaderboard_" + mode;
        auto res = tx.exec_params(
            "SELECT username, speed_wpm, accuracy, played_at FROM " + view + " ORDER BY speed_wpm DESC");
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

    std::string TextDatabase::get_username_by_login(const std::string &login)
    {
        try
        {
            pqxx::work txn(*conn_);
            pqxx::result r = txn.exec_params(
                "SELECT username FROM players WHERE login = $1 LIMIT 1",
                login);
            txn.commit();
            if (r.empty())
                return "";
            return r[0][0].as<std::string>();
        }
        catch (const std::exception &)
        {
            return "";
        }
    }
    std::vector<GameRecord> TextDatabase::get_last_games(int player_id, int limit) const
    {
        verify_connection();
        pqxx::nontransaction tx(*conn_);
        auto res = tx.exec_params(
            "SELECT mode, speed_wpm, accuracy, played_at "
            "FROM games WHERE player_id = $1 "
            "ORDER BY played_at DESC LIMIT $2",
            player_id, limit);

        std::vector<GameRecord> records;
        for (const auto &row : res)
        {
            records.push_back({row["mode"].as<std::string>(),
                               row["speed_wpm"].as<double>(),
                               row["accuracy"].as<double>(),
                               row["played_at"].as<std::string>()});
        }
        return records;
    }

} // namespace server
#pragma GCC diagnostic pop