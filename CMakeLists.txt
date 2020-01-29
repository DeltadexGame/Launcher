project(Launcher C CXX)
cmake_minimum_required(VERSION 2.8.12)

include(cmake/App.cmake)

set(SOURCES "src/Launcher.h"
            "src/Launcher.cpp"
            "src/main.cpp")

add_app("${SOURCES}")