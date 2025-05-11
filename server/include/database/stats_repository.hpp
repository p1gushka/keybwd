#pragma once
#include <nlohmann/json.hpp>
#include "db_connection.hpp"
#include <pqxx/pqxx>

using json = nlohmann::json;

class StatsRepository
{
public:
    static void updateGlobalStats(const json &session_data);
    static json getGlobalStats();
};