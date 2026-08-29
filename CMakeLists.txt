cmake_minimum_required(VERSION 3.24)

project(
    QuantForge
    VERSION 1.0.0
    DESCRIPTION "C++ quantitative finance options pricing engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(QuantForge.Core)
add_subdirectory(QuantForge.CLI)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(QuantForge.Tests)
endif()