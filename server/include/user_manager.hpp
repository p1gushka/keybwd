#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include <string>
#include "user.hpp"

class UserManager {
    User& current_user;
public:
    UserManager(User& user);
    bool login(const std::string& username, const std::string& password);
    bool register_user(const std::string& username, const std::string& password);
};

#endif
