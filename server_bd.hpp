#ifndef SERVER_BD_HPP_
#define SERVER_BD_HPP_

#include <iostream>
#include <pqxx/pqxx>
#include <memory>
namespace server
{
    class Server_db
    {
        std::unique_ptr<pqxx::connection> conn_;

    public:
        Server_db(const std::string &host,
                  const std::string &dbname,
                  const std::string &user,
                  const std::string &password,
                  const std::string &port = "5432");
        void create_table();
        void insert_to_table_text(const std::string &text);
        void print_all_table_text();
        void delete_text_by_id(int id);
        void delete_text_by_content(std::string &content);
        void clear_table_completely();
        std::string get_random_text();
    };
} // server
#endif // SERVER_BD_HPP_