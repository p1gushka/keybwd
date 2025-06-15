cmake_minimum_required(VERSION 3.16)

project(KeyboardTrainer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Политики для автоматической обработки MOC/UIC/RCC
cmake_policy(SET CMP0071 NEW)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# РУЧНАЯ НАСТРОЙКА ДЛЯ WSL: пути к libpqxx и libpq
set(LIBPQXX_INCLUDE_DIRS "/usr/include")
set(LIBPQXX_LIBRARIES "/usr/lib/x86_64-linux-gnu/libpqxx.so" "/usr/lib/x86_64-linux-gnu/libpq.so")

# Подключаем директории с заголовками
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${LIBPQXX_INCLUDE_DIRS}
)

# Собираем все .cpp файлы, включая наши текстовые БД
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/main.cpp"
    "${CMAKE_SOURCE_DIR}/text_database.cpp"
    "${CMAKE_SOURCE_DIR}/model/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/components/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/screens/*.cpp"
)

# Заголовки (*.hpp / *.h) для IDE, но они не участвуют в линковке напрямую
file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/*.hpp"
    "${CMAKE_SOURCE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/model/*.hpp"
    "${CMAKE_SOURCE_DIR}/view/*.hpp"
    "${CMAKE_SOURCE_DIR}/view/components/*.hpp"
    "${CMAKE_SOURCE_DIR}/view/screens/*.hpp"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${LIBPQXX_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# --- GoogleTest ---

option(BUILD_TESTS "Build tests" NO)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_executable(test_text_database
        test/test_text_database.cpp
        ${CMAKE_SOURCE_DIR}/text_database.cpp
    )

    target_include_directories(test_text_database PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${LIBPQXX_INCLUDE_DIRS}
    )

    target_link_libraries(test_text_database
        PRIVATE
        GTest::GTest
        GTest::Main
        ${LIBPQXX_LIBRARIES}
        pthread
    )

    add_test(NAME TextDatabaseTests COMMAND test_text_database)
endif()
