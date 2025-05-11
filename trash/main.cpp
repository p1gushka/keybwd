#include <iostream>
#include "server_bd.hpp"

int main()
{
    try
    {
        server::Server_db db(
            "localhost",
            "textdb",
            "textuser",
            "secure_password");

        std::string s = "Вот парадный подъезд";
        db.insert_to_table_text(s);
        db.insert_to_table_text(s);
        db.insert_to_table_text(s);
        db.delete_text_by_id(14);
        db.delete_text_by_content(s);
        db.print_all_table_text();
        db.clear_table_completely();
        std::cout << db.get_random_text() << std::endl;
        std::cout << db.get_random_text() << std::endl;
        std::cout << db.get_random_text() << std::endl;
        db.print_all_table_text();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
