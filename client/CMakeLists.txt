cmake_minimum_required(VERSION 3.16)

project(KeyboardTrainer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0071 NEW)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/model
    ${CMAKE_SOURCE_DIR}/view
    ${CMAKE_SOURCE_DIR}/view/components
    ${CMAKE_SOURCE_DIR}/view/screens
)

file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/main.cpp"

    "${CMAKE_SOURCE_DIR}/model/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/components/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/screens/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/model/*.h"
    "${CMAKE_SOURCE_DIR}/view/*.h"
    "${CMAKE_SOURCE_DIR}/view/components/*.h"
    "${CMAKE_SOURCE_DIR}/view/screens/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Установка выходной директории
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
