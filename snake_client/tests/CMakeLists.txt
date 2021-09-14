cmake_minimum_required(VERSION 3.14)

project(tests_snake_client)

set(FILES_UT
    snake_client_ut.cpp
    mocks/ui_iface_mock.cpp
    socket_option_fake.cpp
)

add_executable(${PROJECT_NAME} ${FILES_UT})
target_link_libraries(${PROJECT_NAME} snake_core_lib gtest_main gtest gmock pthread)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
