#include "json_work.hpp"

json read_json_from_file(const std::string &filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        json j;
        file >> j;
        return j;
    }
    else
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }
}

void write_json_to_file(const json &j, const std::string &filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << std::setw(4) << j << std::endl;
    }
    else
    {
        throw std::runtime_error("Failed to write to file: " + filename);
    }
}

json create_default_stat_json()
{
    return {
        {"max_speed_tap", 0},
        {"mid_correct_tap", 0},
        {"cnt_vin_mulplay", 0}};
}

json create_default_story_game_json()
{
    return {
        {"speed_tap", 0},
        {"correct_tap", 0},
        {"vin", 0}};
}

json load_or_create_stat_json()
{
    try
    {
        return read_json_from_file(STAT_FILENAME);
    }
    catch (const std::runtime_error &e)
    {
        return create_default_stat_json();
    }
}

json load_or_create_story_game_json()
{
    try
    {
        return read_json_from_file(STORY_FILENAME);
    }
    catch (const std::runtime_error &e)
    {
        return create_default_story_game_json();
    }
}

void write_stat_json(const json &j)
{
    write_json_to_file(j, STAT_FILENAME);
}

void write_story_game_json(const json &j)
{
    write_json_to_file(j, STORY_FILENAME);
}