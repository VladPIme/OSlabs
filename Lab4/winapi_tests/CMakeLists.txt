cmake_minimum_required(VERSION 3.16)
project(winapi_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../winapi/include)

add_executable(winapi_tests
    src/tests.cpp
)

target_link_libraries(winapi_tests 
    message_queue 
    Catch2::Catch2
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)
catch_discover_tests(winapi_tests)