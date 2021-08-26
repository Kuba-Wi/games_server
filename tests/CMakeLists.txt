cmake_minimum_required(VERSION 3.10)

project(tests_server)

set(FILES_UT
    snake_ut.cpp
    timer_ut.cpp
    snake_game_ut.cpp
)

include_directories(..)

add_executable(${PROJECT_NAME} ${FILES_UT})
target_link_libraries(${PROJECT_NAME} server_core_lib gtest_main gtest gmock pthread)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
