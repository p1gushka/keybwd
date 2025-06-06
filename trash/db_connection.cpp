#include "db_connection.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>

void DatabaseConnectionPool::initialize(const std::string &conn_str, size_t pool_size)
{
    std::lock_guard<std::mutex> lock(pool_mutex_);
    connection_string_ = conn_str;

    try
    {
        for (size_t i = 0; i < pool_size; ++i)
        {
            auto conn = std::make_shared<pqxx::connection>(connection_string_);
            if (!conn->is_open())
            {
                throw std::runtime_error("Failed to establish database connection");
            }
            connection_pool_.push_back(conn);
            spdlog::info("Created DB connection #{}", i + 1);
        }
    }
    catch (const std::exception &e)
    {
        spdlog::critical("Database initialization failed: {}", e.what());
        throw;
    }
}

std::shared_ptr<pqxx::connection> DatabaseConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(pool_mutex_);

    if (connection_pool_.empty())
    {
        try
        {
            spdlog::warn("Connection pool empty, creating new connection");
            auto conn = std::make_shared<pqxx::connection>(connection_string_);
            if (!conn->is_open())
            {
                throw std::runtime_error("New connection is not open");
            }
            return conn;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to create new connection: {}", e.what());
            throw std::runtime_error("No available connections and cannot create new");
        }
    }

    auto conn = connection_pool_.back();
    connection_pool_.pop_back();
    return conn;
}

void DatabaseConnectionPool::returnConnection(std::shared_ptr<pqxx::connection> conn)
{
    if (!conn || !conn->is_open())
    {
        spdlog::warn("Returned invalid connection, discarding");
        return;
    }

    std::lock_guard<std::mutex> lock(pool_mutex_);
    connection_pool_.push_back(conn);
}