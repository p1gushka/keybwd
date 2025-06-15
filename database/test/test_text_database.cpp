#include <gtest/gtest.h>
#include <pqxx/pqxx>
#include "text_database.hpp"

using namespace server;

class TextDatabaseTest : public ::testing::Test
{
protected:
    TextDatabase *db;

    void SetUp() override
    {
        db = new TextDatabase("localhost", "textdb", "textuser", "secure_password", "5432");

        // Очистка тестовых данных для чистоты теста, чтобы не мешали старые записи
        try
        {
            pqxx::connection c("host=localhost dbname=textdb user=textuser password=secure_password port=5432");
            pqxx::work txn(c);
            txn.exec("DELETE FROM players WHERE login LIKE 'test_%';");
            txn.exec("DELETE FROM texts WHERE title LIKE 'Тест%';");
            txn.exec("DELETE FROM quotes WHERE content LIKE 'Цитата тестовая%';");
            txn.exec("DELETE FROM code_snippets WHERE title LIKE 'Hello%';");
            txn.exec("DELETE FROM words WHERE word LIKE 'alpha%';");
            txn.commit();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error cleaning test data: " << e.what() << std::endl;
        }
    }

    void TearDown() override
    {
        delete db;
    }
};

TEST_F(TextDatabaseTest, RegisterAndAuthenticatePlayer)
{
    std::string login = "test_user_1";
    std::string username = "Tester";
    std::string password = "secret";

    bool registered = db->register_player(login, username, password);
    EXPECT_TRUE(registered);

    auto pid = db->authenticate_player(login, password);
    EXPECT_TRUE(pid.has_value());
}

TEST_F(TextDatabaseTest, InsertAndFetchText)
{
    db->insert_text(1, "Тест: Пример", "Пример текста для режима На время");

    auto text = db->get_random_text(1);
    ASSERT_FALSE(text.empty()) << "get_random_text вернул пустую строку";
    EXPECT_NE(text.find("Пример текста"), std::string::npos);
}

TEST_F(TextDatabaseTest, InsertAndFetchWord)
{
    // Очистим таблицу слов
    try
    {
        pqxx::connection c("host=localhost dbname=textdb user=textuser password=secure_password port=5432");
        pqxx::work txn(c);
        txn.exec("DELETE FROM words;");
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error cleaning words table: " << e.what() << std::endl;
    }

    db->insert_word("alpha");

    auto words = db->get_random_words(10);
    ASSERT_FALSE(words.empty()) << "get_random_words вернул пустой список";

    // Проверяем, что "alpha" есть среди полученных слов
    auto it = std::find(words.begin(), words.end(), "alpha");
    EXPECT_NE(it, words.end()) << "'alpha' не найден среди слов";
}

TEST_F(TextDatabaseTest, InsertAndFetchQuote)
{
    // Очистим таблицу цитат
    try
    {
        pqxx::connection c("host=localhost dbname=textdb user=textuser password=secure_password port=5432");
        pqxx::work txn(c);
        txn.exec("DELETE FROM quotes;");
        txn.commit();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error cleaning quotes table: " << e.what() << std::endl;
    }

    db->insert_quote("Цитата тестовая", "short", "Автор");

    auto quote = db->get_random_quote("short");
    ASSERT_FALSE(quote.empty()) << "get_random_quote вернул пустую строку";
    EXPECT_NE(quote.find("Цитата"), std::string::npos);
}

TEST_F(TextDatabaseTest, InsertAndFetchCodeSnippet)
{
    db->insert_code_snippet("C++", "Hello", "std::cout << \"Hello\" << std::endl;");

    auto code = db->get_random_code("C++");
    ASSERT_FALSE(code.empty()) << "get_random_code вернул пустую строку";
    EXPECT_NE(code.find("std::cout"), std::string::npos);
}
