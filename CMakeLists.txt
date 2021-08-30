cmake_minimum_required(VERSION 3.10)

project(game_server)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FILES
    snake.cpp
    server.cpp
    servers.cpp
    game_server.cpp
    snake_game.cpp
    send_task.cpp
)
set(COMPILE_FLAGS -Wall -pedantic -Wextra)

add_library(server_core_lib STATIC ${FILES})
target_compile_options(server_core_lib PRIVATE ${COMPILE_FLAGS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} server_core_lib pthread)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})

add_subdirectory(snake_client)
add_subdirectory(tests)
