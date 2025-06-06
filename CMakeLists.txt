cmake_minimum_required(VERSION 3.15)
project(KeyboardTrainer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Общие настройки для всего проекта
add_subdirectory(client)
add_subdirectory(server)
