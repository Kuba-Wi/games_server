cmake_minimum_required(VERSION 3.14)

project(snake LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

set(ANDROID_BOOST_PATH CACHE PATH "Path to Boost for Android")

set(PROJECT_SOURCES_CORE
    src/snake_client.cpp
    src/network.cpp
    src/receive_worker.cpp
    src/send_worker.cpp
)

set(PROJECT_SOURCES
    src/main.cpp
    src/socket_option.cpp
    ui/qt_ui_iface.cpp
    ui/snakeModel.cpp
    ui/qml.qrc
)

set(COMPILE_FLAGS -pedantic -Wall -Wextra)

include_directories(include)
include_directories(ui)

if(ANDROID)
    if(EXISTS ${ANDROID_BOOST_PATH})
        include_directories(${ANDROID_BOOST_PATH})
    else()
        message(FATAL_ERROR "Path to Boost for Android is not specified")
    endif()
    add_library(snake SHARED ${PROJECT_SOURCES} ${PROJECT_SOURCES_CORE})
else()
    add_library(snake_core_lib STATIC ${PROJECT_SOURCES_CORE})
    target_compile_options(snake_core_lib PRIVATE ${COMPILE_FLAGS})

    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(snake ${PROJECT_SOURCES})
    else()
        add_executable(snake ${PROJECT_SOURCES})
    endif()
endif()

target_compile_options(snake PRIVATE ${COMPILE_FLAGS})

target_compile_definitions(snake
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(snake
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

if(NOT ANDROID)
    target_link_libraries(snake PRIVATE snake_core_lib pthread)
endif()

if(BUILD_TESTS)
    add_compile_definitions(snake UNITTEST)
    add_subdirectory(tests)
endif()
