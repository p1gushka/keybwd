#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
    bool is_authenticated;
    std::string username;
public:
    User();
    void login(const std::string& uname);
    void logout();
    bool is_logged_in() const;
    std::string get_username() const;
};

#endif
