#ifndef JSON_WORK_HPP_
#define JSON_WORK_HPP_

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

const std::string STAT_FILENAME = "statictick_game.json";
const std::string STORY_FILENAME = "story_game.json";

json read_json_from_file(const std::string &filename);

void write_json_to_file(const json &j, const std::string &filename);

json create_default_stat_json();

json create_default_story_game_json();

json load_or_create_stat_json();

json load_or_create_story_game_json();

void write_stat_json(const json &j);

void write_story_game_json(const json &j);

#endif // JSON_WORK_HPP_