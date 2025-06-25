cmake_minimum_required(VERSION 3.16)
project(threads_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(../threads/include)

add_executable(threads_tests
    src/tests.cpp
)

target_link_libraries(threads_tests 
    message_queue_threads 
    ${CMAKE_THREAD_LIBS_INIT}
    Catch2::Catch2
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)
catch_discover_tests(threads_tests)