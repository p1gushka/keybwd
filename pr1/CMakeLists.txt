cmake_minimum_required(VERSION 3.16)
project(KeyboardTrainer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic handling of MOC, UIC, and RCC (for Qt)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ------------------------------------------------------------------------------
# Qt6 setup
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# ------------------------------------------------------------------------------
# Find libpqxx using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)

# ------------------------------------------------------------------------------
# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/model
    ${PROJECT_SOURCE_DIR}/view
    ${PROJECT_SOURCE_DIR}/view/screens
    ${PROJECT_SOURCE_DIR}/view/components
    ${CMAKE_SOURCE_DIR}/../trash              # Include for text_database.hpp
)

# ------------------------------------------------------------------------------
# Source files
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/model/*.cpp
    ${PROJECT_SOURCE_DIR}/view/*.cpp
    ${PROJECT_SOURCE_DIR}/view/screens/*.cpp
    ${PROJECT_SOURCE_DIR}/view/components/*.cpp
)

# Add trash/text_database.cpp manually
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/../trash/text_database.cpp)

# ------------------------------------------------------------------------------
# Add executable
add_executable(KeyboardTrainer ${SOURCES})

# ------------------------------------------------------------------------------
# Link libraries
target_link_libraries(KeyboardTrainer
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    PkgConfig::PQXX
)
