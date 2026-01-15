cmake_minimum_required(VERSION 3.21)
project(Line LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/msvc2022_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup()

add_executable(Line
    main.cpp
 "MainWindow.cpp" "MainWindow.h" "TimelineView.cpp" "TimelineView.h"   "NodeItem.h" "NodeItem.cpp" "ConnectionItem.cpp" "ConnectionItem.h"    )

target_link_libraries(Line
    Qt6::Widgets
    Qt6::Sql
)
