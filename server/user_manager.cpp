#include "user_manager.hpp"
#include <fstream>
#include <vector>
#include <algorithm>

UserManager::UserManager(User& user) : current_user(user) {}

bool UserManager::login(const std::string& username, const std::string& password) {
    // mock
    std::ifstream db("users.txt");
    std::string line;
    while (std::getline(db, line)) {
        size_t delim = line.find(':');
        if (line.substr(0, delim) == username && line.substr(delim+1) == password) {
            current_user.login(username);
            return true;
        }
    }
    return false;
}

bool UserManager::register_user(const std::string& username, const std::string& password) {
    // mock
    std::ofstream db("users.txt", std::ios::app);
    db << username << ":" << password << "\n";
    current_user.login(username);
    return true;
}
