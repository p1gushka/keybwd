#ifndef TEXT_DATABASE_HPP
#define TEXT_DATABASE_HPP

#include "database_base.hpp"
#include <string>
#include <optional>
#include <vector>

namespace server
{
    struct AverageStats
    {
        double avg_speed_wpm;
        double avg_raw_wpm;
        double avg_accuracy;
        double avg_correct_symbols;
        double avg_wrong_symbols;
        double avg_missed_symbols;
        double avg_extra_symbols;
    };

    // Добавляем структуру для записи в лидерборд
    struct LeaderboardEntry
    {
        std::string username;
        double speed_wpm;
        double accuracy;
        std::string played_at;
    };

    class TextDatabase : public DatabaseBase
    {
    public:
        TextDatabase(const std::string &host,
                     const std::string &dbname,
                     const std::string &user,
                     const std::string &password,
                     const std::string &port = "5432");

        bool register_player(const std::string &login, const std::string &username, const std::string &password);
        std::optional<int> authenticate_player(const std::string &login, const std::string &password);
        std::string get_username(int player_id) const;
        void refresh_leaderboards();

        void insert_text(const std::string &content, const std::string &title = "Без названия");
        std::string get_random_text() const;
        void print_all() const;
        void delete_by_id(int id);
        void delete_by_content(const std::string &content);
        void clear_table();

        void record_game(int player_id,
                         const std::string &mode,
                         double speed_wpm,
                         double raw_wpm,
                         double accuracy,
                         int correct_symbols,
                         int wrong_symbols,
                         int missed_symbols,
                         int extra_symbols);

        AverageStats get_average_stats(int player_id) const;

        std::vector<LeaderboardEntry> get_leaderboard(const std::string &mode, int limit) const;
    };
}

#endif // TEXT_DATABASE_HPP
