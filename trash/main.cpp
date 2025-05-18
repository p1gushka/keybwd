#include <iostream>
#include "text_database.hpp"

int main()
{
    try
    {
        server::TextDatabase db(
            "localhost",
            "textdb",
            "textuser",
            "secure_password",
            "5432");

        std::string s = "Вот парадный подъезд";

        db.insert_text(s);
        db.insert_text(s);
        db.insert_text(s, "Какой-то текст, для печати хочу научиться быстро печатать, кстати, сейчас я печатаю вслепую)");

        // db.delete_by_id(7);
        // db.delete_by_content(s);

        // db.print_all();
        // db.clear_table();

        std::cout << db.get_random_text() << std::endl;
        std::cout << db.get_random_text() << std::endl;
        std::cout << db.get_random_text() << std::endl;

        db.print_all();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}