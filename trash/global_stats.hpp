#ifndef GLOBAL_STATS_HPP
#define GLOBAL_STATS_HPP

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

class GlobalStats
{
public:
    explicit GlobalStats(const std::string &data_dir = "data");

    void add_session(const json &session_data);
    json get_stats() const;
    void create_backup() const;
    void add_session_from_file(const std::string &file_path);
    unsigned get_total_sessions() const noexcept;
    int get_max_speed() const noexcept;
    double get_avg_accuracy() const noexcept;

private:
    struct StatsCache
    {
        unsigned total_sessions = 0;
        int max_speed = 0;
        double avg_accuracy = 0.0;
    };

    json create_default_stats() const;
    std::string generate_session_id() const;
    void save_session_file(const json &session_data) const;
    void update_cache(const json &data) noexcept;
    bool atomic_save(const json &data) const;
    bool validate_stats(const json &data) const;
    void migrate_stats(json &data) const;
    void safe_remove(const std::string &path) const;

    std::string data_dir_;
    std::string stats_path_;
    mutable std::mutex mtx_;
    std::atomic<StatsCache> cache_;
};

#endif // GLOBAL_STATS_HPP
