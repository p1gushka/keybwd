# Поиск необходимых библиотек
find_package(Boost REQUIRED COMPONENTS system)
find_package(PostgreSQL REQUIRED)

# Настройка исходников сервера
file(GLOB_RECURSE SERVER_SOURCES
    "src/*.cpp"
    "include/*.hpp"
)

# Создание исполняемого файла сервера
add_executable(server 
    ${SERVER_SOURCES}
    src/server.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(server PRIVATE 
    Boost::system
    PostgreSQL::PostgreSQL
)

target_compile_definitions(server PRIVATE 
    -DUSE_POSTGRESQL
)

# Установка выходной директории
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
