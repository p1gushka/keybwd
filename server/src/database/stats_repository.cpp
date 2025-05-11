#include "stats_repository.hpp"
#include "db_connection.hpp"
#include <pqxx/pqxx>
#include <spdlog/spdlog.h>

void StatsRepository::updateGlobalStats(const json &session_data)
{
    auto conn = DatabaseConnectionPool::getConnection();

    try
    {
        pqxx::work txn(*conn);

        // 1. Гарантированное создание записи
        txn.exec(
            "INSERT INTO global_stats (id, max_speed, total_sessions, sessions_list) "
            "VALUES (1, 0, 0, '[]'::jsonb) "
            "ON CONFLICT (id) DO NOTHING");

        // 2. Обновление статистики
        int speed = session_data["metrics"]["speed"].get<int>();
        std::string session_id = session_data["session_id"].get<std::string>();

        txn.exec_params(
            "UPDATE global_stats SET "
            "total_sessions = total_sessions + 1, "
            "max_speed = GREATEST(max_speed, $1), "
            "sessions_list = sessions_list || $2::jsonb "
            "WHERE id = 1",
            speed,
            json::array({session_id}).dump());

        // 3. Обновление средней точности
        txn.exec(
            "UPDATE global_stats SET average_accuracy = ("
            "SELECT COALESCE(AVG((metrics->>'accuracy')::numeric), 0.0) "
            "FROM sessions"
            ") WHERE id = 1");

        txn.commit();
        DatabaseConnectionPool::returnConnection(conn);
    }
    catch (...)
    {
        DatabaseConnectionPool::returnConnection(conn);
        throw;
    }
}

json StatsRepository::getGlobalStats()
{
    auto conn = DatabaseConnectionPool::getConnection();

    try
    {
        pqxx::nontransaction ntx(*conn);
        auto result = ntx.exec("SELECT * FROM global_stats WHERE id = 1");

        DatabaseConnectionPool::returnConnection(conn);

        if (result.empty())
        {
            return {
                {"version", 3},
                {"total_sessions", 0},
                {"max_speed", 0},
                {"average_accuracy", 0.0},
                {"sessions", json::array()}};
        }

        return {
            {"version", 3},
            {"total_sessions", result[0]["total_sessions"].as<int>()},
            {"max_speed", result[0]["max_speed"].as<int>()},
            {"average_accuracy", result[0]["average_accuracy"].as<double>()},
            {"sessions", json::parse(result[0]["sessions_list"].as<std::string>())}};
    }
    catch (...)
    {
        DatabaseConnectionPool::returnConnection(conn);
        throw;
    }
}