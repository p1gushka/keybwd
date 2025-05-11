#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void do_session(tcp::socket socket)
{
    try
    {
        beast::flat_buffer buffer;

        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        std::string client_id = req.body();
        std::cout << "Клиент \"" << client_id << "\" подключился" << std::endl;

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "server");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "Привет, " + client_id + "! Ты подключился к серверу на порте 8080.";
        res.prepare_payload();

        http::write(socket, res);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main()
{
    try
    {
        net::io_context ioc;

        tcp::acceptor acceptor{ioc, {net::ip::make_address("127.0.0.1"), 8080}};

        std::cout << "Сервер работает на порту 8080..." << std::endl;

        while (true)
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread(do_session, std::move(socket)).detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Исключение: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
