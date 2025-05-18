cmake_minimum_required(VERSION 3.15)
project(TextDatabaseApp)

find_package(libpqxx REQUIRED)

add_executable(text_database_app
    src/main.cpp
    src/text_database.cpp
)

target_include_directories(text_database_app PRIVATE include)
target_link_libraries(text_database_app PRIVATE libpqxx)