cmake_minimum_required(VERSION 3.20)
project(KeyboardTrainerServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS  OFF)

# Находим POCO (если установлен системно или через vcpkg/Conan)
find_package(Poco REQUIRED COMPONENTS Net Util Foundation)

# Подключаем libpqxx (через pkg-config)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Файлы исходников
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/handlers/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME}
    ${SERVER_SOURCES}
)

# Пути к заголовкам
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PQXX_INCLUDE_DIRS}
)

# Линковка библиотек
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pqxx
        Poco::Net
        Poco::Util
        Poco::Foundation
)
