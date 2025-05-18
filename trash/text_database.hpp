#ifndef TEXT_DATABASE_HPP
#define TEXT_DATABASE_HPP

#include "database_base.hpp"
#include <string>

namespace server
{

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
    };

} // namespace server

#endif // TEXT_DATABASE_HPP