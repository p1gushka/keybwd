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

#include "server_bd.hpp"

namespace server
{
    Server_db::Server_db(const std::string &host,
                         const std::string &dbname,
                         const std::string &user,
                         const std::string &password,
                         const std::string &port)
    {
        std::string connection_str =
            "host=" + host + " " +
            "dbname=" + dbname + " " +
            "user=" + user + " " +
            "password=" + password + " " +
            "port=" + port;

        conn_ = std::make_unique<pqxx::connection>(connection_str);

        if (!conn_->is_open())
        {
            throw std::runtime_error("Database connection failed");
        }
        std::cout << "Database connected successfully" << std::endl;
    }

    void Server_db::insert_to_table_text(const std::string &text)
    {
        pqxx::work tx_ins(*conn_);
        tx_ins.exec_params(
            "INSERT INTO texts (content) VALUES ($1)",
            text);
        tx_ins.commit();
        std::cout << "Data added successfully." << std::endl;
    }

    void Server_db::print_all_table_text()
    {
        pqxx::nontransaction tx_r(*conn_);
        pqxx::result res = tx_r.exec("SELECT * FROM texts");
        if (res.empty())
        {
            std::cout << "The table is empty" << std::endl;
            return;
        }
        std::cout << "\nTable Contents:\n";

        for (const auto &row : res)
        {
            std::cout << "ID: " << row["id"].as<int>()
                      << "\nText: " << row["content"].as<std::string>()
                      << "\nDate: " << row["created_at"].as<std::string>()
                      << "\n----------------------\n";
        }
        std::cout << std::endl;
    }

    std::string Server_db::get_random_text()
    {
        pqxx::nontransaction tx_g(*conn_);
        pqxx::result res = tx_g.exec(
            "SELECT content FROM texts "
            "ORDER BY RANDOM() "
            "LIMIT 1");

        return res.empty() ? "" : res[0][0].as<std::string>();
    }

    void Server_db::delete_text_by_id(int id)
    {
        pqxx::work tx_d(*conn_);
        pqxx::result res = tx_d.exec_params("DELETE FROM texts WHERE id = $1 RETURNING *", id);
        tx_d.commit();
        if (res.empty())
        {
            std::cout << "There is no record with ID " << id << std::endl;
            return;
        }
        std::cout << "Text with ID " << id << " deleted" << std::endl;
    }

    void Server_db::delete_text_by_content(std::string &content)
    {
        pqxx::work tx_d(*conn_);
        pqxx::result res = tx_d.exec_params("DELETE FROM texts WHERE content = $1 RETURNING *", content);
        tx_d.commit();
        std::cout << res.size() << " entries were deleted" << std::endl;
    }

    void Server_db::clear_table_completely()
    {
        pqxx::work tx(*conn_);
        tx.exec("TRUNCATE TABLE texts RESTART IDENTITY");
        tx.commit();
        std::cout << "The table is completely cleared" << std::endl;
    }
} // server