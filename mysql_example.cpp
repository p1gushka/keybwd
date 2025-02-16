#include <iostream>
#include <mysqlx/xdevapi.h>

int main()
{
    try
    {
        std::string host = "localhost";
        int port = 3306;
        std::string user = "root";
        std::string password = "ваш_пароль";
        std::string schema = "my_database";

        mysqlx::Session sess(host, port, user, password, schema);

        mysqlx::Schema db = sess.getSchema(schema);

        mysqlx::Table myTable = db.getTable("users");

        mysqlx::RowResult result = myTable.select("name", "email", "age").execute();

        std::cout << "Users:" << std::endl;
        for (mysqlx::Row row : result)
        {
            std::cout << "Name: " << row[0] << ", Email: " << row[1] << ", Age: " << row[2] << std::endl;
        }

        sess.close();
    }
    catch (const mysqlx::exception &e)
    {
        std::cerr << "Ошибка MySQL: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Общая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}