cmake_minimum_required(VERSION 3.7)
project(gap-tinyframe C)
set(CMAKE_CXX_STANDARD GNU17)

# TinyFrame source files are vanilla.
# The TinyFrame configuration file, however, has been written by us.
set(
    GAP_TINYFRAME_SRC
    ${PROJECT_SOURCE_DIR}/../TinyFrame/TinyFrame.c
)
include_directories(include)

add_library(gaptinyframe STATIC ${GAP_TINYFRAME_SRC})
