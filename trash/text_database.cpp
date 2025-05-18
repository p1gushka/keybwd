/*
sudo -u postgres psql -c "CREATE USER textuser WITH PASSWORD 'secure_password';"
sudo -u postgres psql -c "CREATE DATABASE textdb OWNER textuser;"

psql -U postgres -d textdb -c "
GRANT CONNECT ON DATABASE textdb TO textuser;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO textuser;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO textuser;
GRANT ALL PRIVILEGES ON DATABASE textdb TO textuser;
"

*/

#include "text_database.hpp"
#include <iostream>

namespace server
{

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

    void TextDatabase::insert_text(const std::string &content, const std::string &title)
    {
        verify_connection();
        pqxx::work tx(*conn_);

        try
        {
            tx.exec_params(
                "INSERT INTO texts (title, content) VALUES ($1, $2)",
                title, content);
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

        auto res = tx.exec("SELECT id, title, content, created_at FROM texts");
        if (res.empty())
        {
            std::cout << "Empty table";
        }
        for (const auto &row : res)
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
        std::cout << (res.empty() ? "Nothing deleted" : "Deleted text ID " + std::to_string(id)) << "\n";
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

} // namespace server