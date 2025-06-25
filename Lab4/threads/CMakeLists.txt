cmake_minimum_required(VERSION 3.16)
project(threads_lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(include)

add_library(message_queue_threads
    include/message_queue.h
    src/message_queue.cpp
)

add_executable(receiver_threads
    src/receiver.cpp
)
target_link_libraries(receiver_threads 
    message_queue_threads 
    ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(sender_threads
    src/sender.cpp
)
target_link_libraries(sender_threads 
    message_queue_threads 
    ${CMAKE_THREAD_LIBS_INIT}
)