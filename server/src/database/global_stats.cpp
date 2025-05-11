#include "global_stats.hpp"
#include "stats_repository.hpp"
#include <chrono>
#include <cmath>
#include <spdlog/spdlog.h>
#include <iomanip>
#include <sstream>

using namespace std::chrono;

GlobalStats::GlobalStats(const std::string &data_dir)
    : data_dir_(data_dir),
      stats_path_((fs::path(data_dir) / "global_stats.json").string())
{
    fs::create_directories(data_dir_ + "/sessions");

    try
    {
        if (fs::exists(stats_path_))
        {
            if (fs::file_size(stats_path_) == 0)
            {
                spdlog::warn("Stats file is empty, recreating...");
                atomic_save(create_default_stats());
            }
            else
            {
                std::ifstream in(stats_path_);
                json data = json::parse(in);
                update_cache(data);
                return;
            }
        }
    }
    catch (...)
    {
        spdlog::warn("Corrupted stats file, recreating...");
    }

    atomic_save(create_default_stats());
}

json GlobalStats::create_default_stats() const
{
    return {
        {"version", 3},
        {"total_sessions", 0},
        {"max_speed", 0},
        {"average_accuracy", 0.0},
        {"sessions", json::array()}};
}
std::string GlobalStats::generate_session_id() const
{
    auto now = system_clock::now();
    auto now_ms = time_point_cast<milliseconds>(now);
    auto time = system_clock::to_time_t(now);
    auto tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

void GlobalStats::save_session_file(const json &session_data) const
{
    if (!session_data.contains("session_id"))
    {
        throw std::runtime_error("Session data missing session_id");
    }

    std::string filename = session_data["session_id"].get<std::string>() + ".json";
    std::string path = (fs::path(data_dir_) / "sessions" / filename).string();

    std::ofstream out(path);
    if (!out)
    {
        throw std::runtime_error("Failed to create session file: " + path);
    }
    out << session_data.dump(4);
    spdlog::info("Session saved to: {}", path);
}

void GlobalStats::update_cache(const json &data) noexcept
{
    StatsCache new_cache;
    try
    {
        new_cache.total_sessions = data["total_sessions"];
        new_cache.max_speed = data["max_speed"];
        new_cache.avg_accuracy = data["average_accuracy"];
        cache_.store(new_cache);
    }
    catch (...)
    {
        spdlog::warn("Cache update failed");
    }
}

void GlobalStats::add_session(const json &session_data)
{
    std::lock_guard<std::mutex> lock(mtx_);

    json session = session_data;

    if (!session.contains("session_id"))
    {
        session["session_id"] = generate_session_id();
    }

    auto now = time_point_cast<milliseconds>(system_clock::now());
    if (!session.contains("start_time"))
    {
        session["start_time"] = now.time_since_epoch().count();
    }
    session["end_time"] = now.time_since_epoch().count();

    try
    {
        save_session_file(session);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save session file: {}", e.what());
        throw;
    }

    json current_stats;
    try
    {
        std::ifstream in(stats_path_);
        current_stats = json::parse(in);
    }
    catch (...)
    {
        current_stats = {
            {"version", 3},
            {"total_sessions", 0},
            {"max_speed", 0},
            {"average_accuracy", 0.0},
            {"sessions", json::array()}};
    }

    unsigned new_total = current_stats["total_sessions"].get<unsigned>() + 1;
    current_stats["total_sessions"] = new_total;

    if (session.contains("metrics"))
    {
        const auto &metrics = session["metrics"];

        if (metrics.contains("speed"))
        {
            int new_speed = metrics["speed"];
            current_stats["max_speed"] = std::max(
                current_stats["max_speed"].get<int>(),
                new_speed);
        }

        if (metrics.contains("accuracy"))
        {
            double new_acc = metrics["accuracy"];
            double current_avg = current_stats["average_accuracy"];
            current_stats["average_accuracy"] =
                (current_avg * (new_total - 1) + new_acc) / new_total;
        }
    }

    current_stats["sessions"].push_back(session["session_id"]);

    try
    {
        StatsRepository::updateGlobalStats(session);
        if (!atomic_save(current_stats))
        {
            throw std::runtime_error("Failed to save stats");
        }
        update_cache(current_stats);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to update stats: {}", e.what());
        throw;
    }
}

void GlobalStats::add_session_from_file(const std::string &file_path)
{
    try
    {
        std::ifstream in(file_path);
        if (!in.is_open())
        {
            spdlog::warn("Failed to open session file: {}", file_path);
            return;
        }

        json session_data = json::parse(in);

        if (!session_data.contains("metrics") || !session_data.contains("session_id"))
        {
            spdlog::warn("Invalid session format in file: {}", file_path);
            return;
        }

        add_session(session_data);
        spdlog::debug("Processed session file: {}", file_path);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error processing {}: {}", file_path, e.what());
    }
}

bool GlobalStats::atomic_save(const json &data) const
{
    const std::string tmp_path = stats_path_ + ".tmp";

    try
    {
        fs::create_directories(fs::path(stats_path_).parent_path());

        {
            std::ofstream out(tmp_path);
            if (!out)
            {
                spdlog::error("Failed to open temp file for writing: {}", tmp_path);
                return false;
            }
            out << data.dump(4);
        }

        fs::rename(tmp_path, stats_path_);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to save stats: {}", e.what());
        safe_remove(tmp_path);
        return false;
    }
}

void GlobalStats::migrate_stats(json &data) const
{
    if (!data.contains("version"))
    {
        data["version"] = 3;
        return;
    }

    int version = data["version"];
    if (version < 2)
    {
        data["average_accuracy"] = 0.0;
        version = 2;
    }
    if (version < 3)
    {
        data["version"] = 3;
    }
}

bool GlobalStats::validate_stats(const json &data) const
{
    return data.contains("version") &&
           data.contains("total_sessions") &&
           data.contains("max_speed") &&
           data.contains("average_accuracy") &&
           data.contains("sessions");
}

void GlobalStats::safe_remove(const std::string &path) const
{
    try
    {
        if (fs::exists(path))
        {
            fs::remove(path);
        }
    }
    catch (...)
    {
        spdlog::warn("Failed to remove file: {}", path);
    }
}

json GlobalStats::get_stats() const
{
    StatsCache current = cache_.load();
    json result = {
        {"version", 3},
        {"total_sessions", current.total_sessions},
        {"max_speed", current.max_speed},
        {"average_accuracy", current.avg_accuracy}};

    try
    {
        std::ifstream in(stats_path_);
        json file_data = json::parse(in);

        if (file_data.contains("sessions"))
        {
            result["sessions"] = file_data["sessions"];
        }
    }
    catch (...)
    {
        spdlog::warn("Couldn't read sessions list");
    }

    return result;
}

void GlobalStats::create_backup() const
{
    auto now = system_clock::now();
    std::string backup_dir = "backups/" +
                             std::to_string(time_point_cast<hours>(now).time_since_epoch().count() / 24);

    try
    {
        fs::create_directories(backup_dir);
        fs::copy(stats_path_,
                 backup_dir + "/stats_" +
                     std::to_string(now.time_since_epoch().count()) + ".json");
    }
    catch (...)
    {
        spdlog::error("Backup creation failed");
        throw;
    }
}

unsigned GlobalStats::get_total_sessions() const noexcept
{
    return cache_.load().total_sessions;
}

int GlobalStats::get_max_speed() const noexcept
{
    return cache_.load().max_speed;
}

double GlobalStats::get_avg_accuracy() const noexcept
{
    return cache_.load().avg_accuracy;
}