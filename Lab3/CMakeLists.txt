cmake_minimum_required(VERSION 3.10)
project(Lab3 LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем тестирование
enable_testing()

# Создаем директорию include, если её нет
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

# Скачиваем Catch2, если его нет
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/catch.hpp)
    message(STATUS "Downloading Catch2 header...")
    file(DOWNLOAD 
         "https://github.com/catchorg/Catch2/releases/download/v2.13.7/catch.hpp"
         ${CMAKE_CURRENT_SOURCE_DIR}/include/catch.hpp
         SHOW_PROGRESS
         STATUS DOWNLOAD_STATUS)
         
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download Catch2 header. Please download it manually and place it in the include directory.")
    endif()
endif()

# Создаем директорию для вывода
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Добавляем подпроекты
add_subdirectory(winapi)
add_subdirectory(winapi_tests)
add_subdirectory(threads)
add_subdirectory(threads_tests)