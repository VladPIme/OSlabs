cmake_minimum_required(VERSION 3.10)
project(ThreadProcessing)

# �������� ��������� C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ��������� ���� ��� �������� ������
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ����� ����������
if(MSVC)
    # ��� Visual Studio
    add_compile_options(/W4)
else()
    # ��� GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# �������� �������� ����� ��������� �������
set(MAIN_SRC
    ${CMAKE_SOURCE_DIR}/Main/��������\ �����/Main.cpp
    ${CMAKE_SOURCE_DIR}/Main/��������\ �����/ThreadProcessing.h
)

# �������� ����������� ����
add_executable(ThreadProcessor ${MAIN_SRC})

# ����� Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # ���� Google Test �� ������, ��������� ��� �������
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # ��� ����� ������ ������ CMake
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# ������� ���������� ��� ������������
add_library(ThreadProcessingLib STATIC
    ${CMAKE_SOURCE_DIR}/Main/��������\ �����/Main.cpp
)
target_include_directories(ThreadProcessingLib PUBLIC
    ${CMAKE_SOURCE_DIR}/Main/��������\ �����
)

# �������� �����
enable_testing()

# �������� ����� ��� ������
set(TEST_SRC
    ${CMAKE_SOURCE_DIR}/Test/pch.cpp
    ${CMAKE_SOURCE_DIR}/Test/pch.h
    ${CMAKE_SOURCE_DIR}/Test/test.cpp
)

# ����������� ���� ��� ������
add_executable(ThreadProcessingTests ${TEST_SRC})
target_link_libraries(ThreadProcessingTests PRIVATE 
    ThreadProcessingLib 
    GTest::gtest 
    GTest::gtest_main
)
target_include_directories(ThreadProcessingTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Main/��������\ �����
)

# �������������� ����� ������ � ���������� � CTest
include(GoogleTest)
gtest_discover_tests(ThreadProcessingTests)