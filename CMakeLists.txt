cmake_minimum_required(VERSION 3.10)

project(games_server)

option(BUILD_TESTS "Enable tests." OFF)
option(BUILD_CLIENT "Enable building client app." ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)
find_package(OpenSSL REQUIRED)

set(FILES
    src/snake.cpp
    src/server.cpp
    src/servers.cpp
    src/game_server.cpp
    src/snake_game.cpp
    src/accept_task.cpp
    src/timeout_task.cpp

    src/privileged_accept_task.cpp
    src/privileged_connection.cpp
    src/privileged_server.cpp

    src/program_options.cpp
)

set(COMPILE_FLAGS -Wall -pedantic -Wextra)
include_directories(include)

add_library(server_core_lib STATIC ${FILES})
target_compile_options(server_core_lib PRIVATE ${COMPILE_FLAGS})

add_executable(${PROJECT_NAME} src/main.cpp src/connection_initializers.cpp)
target_link_libraries(${PROJECT_NAME} server_core_lib pthread spdlog ${OPENSSL_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})

if(BUILD_TESTS)
    add_compile_definitions(${PROJECT_NAME} UNITTEST)
    add_subdirectory(tests)
endif()

if(BUILD_CLIENT)
    add_subdirectory(snake_client)
endif()
