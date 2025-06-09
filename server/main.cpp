#include "server.hpp"

int main() {
    HttpServer server(9090);
    
    server.start();

    std::cout << "Press Enter to stop the server..." << "\n";
    std::cin.get();

    server.stop();

    return 0;
}