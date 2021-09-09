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

set(PROJECT_SOURCES
        src/main.cpp
        src/snake_client.cpp
        src/network.cpp
        src/receive_worker.cpp
        src/send_worker.cpp
        ui/qt_ui_iface.cpp
        ui/snakeModel.cpp
        ui/qml.qrc
)

set(FLAGS -pedantic -Wall -Wextra)

include_directories(include)
include_directories(ui)

if(ANDROID)
    include_directories(/home/kuba/Boost-for-Android/boost_1_76_0)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(snake
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(snake SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(snake
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(snake
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(snake
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
target_compile_options(snake PRIVATE ${FLAGS})

if(NOT ANDROID)
    target_link_libraries(snake PRIVATE pthread)
endif()
