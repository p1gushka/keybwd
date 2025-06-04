cmake_minimum_required(VERSION 3.16)

project(KeyboardTrainer VERSION 1.0 LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Политики для автоматической обработки MOC/UIC/RCC
cmake_policy(SET CMP0071 NEW)

# Используем Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# --- РУЧНАЯ НАСТРОЙКА ДЛЯ WSL ---
# Пути к заголовкам и библиотекам в WSL (Ubuntu)
set(LIBPQXX_INCLUDE_DIRS "/usr/include")
set(LIBPQXX_LIBRARIES "/usr/lib/x86_64-linux-gnu/libpqxx.so" "/usr/lib/x86_64-linux-gnu/libpq.so")

# Подключаем директории с заголовками
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/model
    ${CMAKE_SOURCE_DIR}/view
    ${CMAKE_SOURCE_DIR}/view/components
    ${CMAKE_SOURCE_DIR}/view/screens
    ${LIBPQXX_INCLUDE_DIRS}  # Добавляем ручной путь
)

# Собираем исходники
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/main.cpp"
    "${CMAKE_SOURCE_DIR}/model/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/components/*.cpp"
    "${CMAKE_SOURCE_DIR}/view/screens/*.cpp"
)

# Собираем заголовки
file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/model/*.h"
    "${CMAKE_SOURCE_DIR}/view/*.h"
    "${CMAKE_SOURCE_DIR}/view/components/*.h"
    "${CMAKE_SOURCE_DIR}/view/screens/*.h"
)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Линкуем библиотеки
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${LIBPQXX_LIBRARIES}  # Ручное указание библиотек
)

# Автоматическая генерация MOC/UIC/RCC
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
