#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int main()
{
    try
    {
        net::io_context ioc;
        tcp::socket socket(ioc);

        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("127.0.0.1", "8080");
        net::connect(socket, results.begin(), results.end());

        http::request<http::string_body> req1{http::verb::post, "/", 11};
        req1.set(http::field::host, "127.0.0.1");
        req1.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req1.set(http::field::content_type, "text/plain");
        req1.body() = "client1";
        req1.prepare_payload();

        http::write(socket, req1);

        beast::flat_buffer buffer;

        http::response<http::string_body> res;
        http::read(socket, buffer, res);

        std::cout << "Ответ от сервера: " << res.body() << std::endl;

        socket.shutdown(tcp::socket::shutdown_both);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
