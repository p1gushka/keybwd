#ifndef TEXT_DATABASE_HPP
#define TEXT_DATABASE_HPP

#include "database_base.hpp"
#include <string>

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

    class TextDatabase : public DatabaseBase
    {
    public:
        TextDatabase(const std::string &host,
                     const std::string &dbname,
                     const std::string &user,
                     const std::string &password,
                     const std::string &port = "5432");

        void insert_text(const std::string &content, const std::string &title = "Без названия");
        std::string get_random_text() const;
        void print_all() const;
        void delete_by_id(int id);
        void delete_by_content(const std::string &content);
        void clear_table();

        int get_or_create_player(const std::string &username);
        void record_game(const std::string &username,
                         double speed_wpm,
                         double raw_wpm,
                         double accuracy,
                         int correct_symbols,
                         int wrong_symbols,
                         int missed_symbols,
                         int extra_symbols);
        AverageStats get_average_stats(const std::string &username) const;
    };

} // namespace server

#endif // TEXT_DATABASE_HPP
