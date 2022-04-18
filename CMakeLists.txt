cmake_minimum_required(VERSION 3.21)

project(CppPerfettoTrace VERSION 1.0.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CPP_PERFETTO_TRACE_BuildExample "Build an executable that generates an example trace file." OFF)

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(fmt)

add_library(CppPerfettoTrace OBJECT
    PerfettoTracing.cpp
    PerfettoTracing.h
    README.md
)

# Temp until compilers have support for std::format
target_link_libraries(CppPerfettoTrace
    PRIVATE
    fmt::fmt
)

if (CPP_PERFETTO_TRACE_BuildExample)
    add_subdirectory(example)
endif()

