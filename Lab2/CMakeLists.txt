cmake_minimum_required(VERSION 3.10)
project(ThreadProcessing)

# Включаем поддержку C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка пути для выходных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Опции компиляции
if(MSVC)
    # Для Visual Studio
    add_compile_options(/W4)
else()
    # Для GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Включаем исходные файлы основного проекта
set(MAIN_SRC
    ${CMAKE_SOURCE_DIR}/Main/Исходные\ файлы/Main.cpp
    ${CMAKE_SOURCE_DIR}/Main/Исходные\ файлы/ThreadProcessing.h
)

# Основной исполняемый файл
add_executable(ThreadProcessor ${MAIN_SRC})

# Поиск Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # Если Google Test не найден, загружаем его вручную
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # Для более старых версий CMake
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Создаем библиотеку для тестирования
add_library(ThreadProcessingLib STATIC
    ${CMAKE_SOURCE_DIR}/Main/Исходные\ файлы/Main.cpp
)
target_include_directories(ThreadProcessingLib PUBLIC
    ${CMAKE_SOURCE_DIR}/Main/Исходные\ файлы
)

# Включаем тесты
enable_testing()

# Исходные файлы для тестов
set(TEST_SRC
    ${CMAKE_SOURCE_DIR}/Test/pch.cpp
    ${CMAKE_SOURCE_DIR}/Test/pch.h
    ${CMAKE_SOURCE_DIR}/Test/test.cpp
)

# Исполняемый файл для тестов
add_executable(ThreadProcessingTests ${TEST_SRC})
target_link_libraries(ThreadProcessingTests PRIVATE 
    ThreadProcessingLib 
    GTest::gtest 
    GTest::gtest_main
)
target_include_directories(ThreadProcessingTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Main/Исходные\ файлы
)

# Автоматический поиск тестов и добавление к CTest
include(GoogleTest)
gtest_discover_tests(ThreadProcessingTests)