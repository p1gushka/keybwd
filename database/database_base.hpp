#ifndef DATABASE_BASE_HPP
#define DATABASE_BASE_HPP

#include <memory>
#include <pqxx/pqxx>
#include <stdexcept>

namespace server
{

    class DatabaseBase
    {
    protected:
        std::unique_ptr<pqxx::connection> conn_;

        void verify_connection() const
        {
            if (!conn_ || !conn_->is_open())
            {
                throw std::runtime_error("Database connection is not established");
            }
        }

    public:
        DatabaseBase(const std::string &conn_str)
        {
            try
            {
                conn_ = std::make_unique<pqxx::connection>(conn_str);
            }
            catch (const std::exception &e)
            {
                throw std::runtime_error("Connection failed: " + std::string(e.what()));
            }
        }

        virtual ~DatabaseBase() = default;

        DatabaseBase(const DatabaseBase &) = delete;
        DatabaseBase &operator=(const DatabaseBase &) = delete;
    };

} // namespace server

#endif // DATABASE_BASE_HPP