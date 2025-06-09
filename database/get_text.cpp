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

// Реализация класса-обёрки будет позже
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <iostream>
#include <pqxx/pqxx>
#include <memory>

std::unique_ptr<pqxx::connection> connect_to_db()
{
    auto conn = std::make_unique<pqxx::connection>(
        "host=localhost "
        "dbname=textdb "
        "user=textuser "
        "password=secure_password "
        "port=5432");

    if (!conn->is_open())
    {
        throw std::runtime_error("Database connection error");
    }
    std::cout << "Database connected successfully" << std::endl;
    return conn;
}
void create_table(pqxx::connection &conn)
{
    pqxx::work tx_c(conn);

    tx_c.exec(
        "CREATE TABLE IF NOT EXISTS texts ("
        "id SERIAL PRIMARY KEY, "
        "content TEXT NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")");

    tx_c.commit();
    std::cout << "The \'....\' table has been created or already exists." << std::endl;
}

void insert_to_table_text(pqxx::connection &conn, const std::string &text)
{
    pqxx::work tx_ins(conn);
    tx_ins.exec_params(
        "INSERT INTO texts (content) VALUES ($1)",
        text);
    tx_ins.commit();
    std::cout << "Data added successfully." << std::endl;
}

void print_all_table_text(pqxx::connection &conn)
{
    pqxx::nontransaction tx_r(conn);
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

std::string get_random_text(pqxx::connection &conn)
{
    pqxx::nontransaction tx_g(conn);
    pqxx::result res = tx_g.exec(
        "SELECT content FROM texts "
        "ORDER BY RANDOM() "
        "LIMIT 1");

    return res.empty() ? "" : res[0][0].as<std::string>();
}

void delete_text_by_id(pqxx::connection &conn, int id)
{
    pqxx::work tx_d(conn);
    pqxx::result res = tx_d.exec_params("DELETE FROM texts WHERE id = $1 RETURNING *", id);
    tx_d.commit();
    if (res.empty())
    {
        std::cout << "There is no record with ID " << id << std::endl;
        return;
    }
    std::cout << "Text with ID " << id << " deleted" << std::endl;
}

void delete_text_by_content(pqxx::connection &conn, std::string &content)
{
    pqxx::work tx_d(conn);
    pqxx::result res = tx_d.exec_params("DELETE FROM texts WHERE content = $1 RETURNING *", content);
    tx_d.commit();
    std::cout << res.size() << " entries were deleted" << std::endl;
}

void clear_table_completely(pqxx::connection &conn)
{
    pqxx::work tx(conn);
    tx.exec("TRUNCATE TABLE texts RESTART IDENTITY");
    tx.commit();
    std::cout << "The table is completely cleared" << std::endl;
}

int main()
{
    try
    {
        auto connect = connect_to_db();
        std::string s = "Вот парадный подъезд";
        // insert_to_table_text(*connect, s);
        // insert_to_table_text(*connect, s);
        // insert_to_table_text(*connect, s);
        // delete_text_by_id(*connect, 7);
        // delete_text_by_content(*connect, s);
        // print_all_table_text(*connect);
        // clear_table_completely(*connect);
        std::cout << get_random_text(*connect) << std::endl;
        std::cout << get_random_text(*connect) << std::endl;
        std::cout << get_random_text(*connect) << std::endl;
        // print_all_table_text(*connect);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
#pragma GCC diagnostic pop