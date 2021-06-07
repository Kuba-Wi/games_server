cmake_minimum_required(VERSION 3.10)

project(game_server)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FILES
    snake.cpp
    server.cpp
    game_server.cpp
    snake_game.cpp
)
set(FLAGS -Wall -pedantic -Wextra)

add_library(lib_core STATIC ${FILES})
target_compile_options(lib_core PRIVATE ${FLAGS})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} lib_core pthread)
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})

add_executable(client client_udp_tutorial.cpp)
target_link_libraries(client -pthread)
target_compile_options(client PRIVATE ${FLAGS})
