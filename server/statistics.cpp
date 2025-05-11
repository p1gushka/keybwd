#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <string>

class Statistics {
public:
    void load_stats(const std::string& username);
    void update_stats(const std::string& username, int wpm, int accuracy);
};

#endif
