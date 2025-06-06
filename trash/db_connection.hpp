#ifndef DP_CON_
#define DP_CON_
#include <pqxx/pqxx>
#include <memory>
#include <string>
#include <mutex>
#include <vector>

class DatabaseConnectionPool
{
public:
    static void initialize(const std::string &conn_str, size_t pool_size = 5);
    static std::shared_ptr<pqxx::connection> getConnection();
    static void returnConnection(std::shared_ptr<pqxx::connection> conn);

    DatabaseConnectionPool() = delete;
    DatabaseConnectionPool(const DatabaseConnectionPool &) = delete;
    DatabaseConnectionPool &operator=(const DatabaseConnectionPool &) = delete;

private:
    static inline std::mutex pool_mutex_;
    static inline std::vector<std::shared_ptr<pqxx::connection>> connection_pool_;
    static inline std::string connection_string_;
};

#endif // DP_CON_
