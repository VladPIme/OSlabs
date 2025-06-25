cmake_minimum_required(VERSION 3.16)
project(winapi_lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(message_queue
    include/message_queue.h
    src/message_queue.cpp
)

add_executable(receiver
    src/receiver.cpp
)
target_link_libraries(receiver message_queue)

add_executable(sender
    src/sender.cpp
)
target_link_libraries(sender message_queue)