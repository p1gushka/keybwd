#include "server.hpp"
#include "request_router.hpp"
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Thread.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>

HttpServer::HttpServer(int port)
    : port(port),
      server(nullptr),
      db("localhost", "textdb", "textuser", "secure_password", "5432")
{}

void HttpServer::start() {
    try {
        std::optional<int> test = db.authenticate_player("",""); 
        std::cout << "DB connection OK\n";
        
        Poco::Net::ServerSocket serverSocket(port);

        Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams;
        params->setMaxThreads(16);

        server::TextDatabase db("localhost", "textdb", "textuser", "secure_password", "5432");
        if (!db.authenticate_player("", "")) {
            std::cerr << "DB not ready\n";
            return;
        }

        auto* router = new server::RequestRouter(db);
        server = new Poco::Net::HTTPServer(router, serverSocket, params);

        server->start();
        std::cout << "Server started on port " << port << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << "\n";
    }
}

void HttpServer::stop() {
    if (server) {
        server->stop();
        std::cout << "Server stopped" << "\n";
    }
}