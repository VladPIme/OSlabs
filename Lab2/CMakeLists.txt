cmake_minimum_required(VERSION 3.14)
project(ThreadsProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- WinAPI версия (Lab2) ----------

# Создаем библиотеку для WinAPI версии
add_library(WinAPIThreadLib
    Lab2/ThreadProcessing.cpp
)

target_include_directories(WinAPIThreadLib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Lab2
)

# Для Windows нужна библиотека Kernel32
if(WIN32)
    target_link_libraries(WinAPIThreadLib PRIVATE Kernel32)
endif()

# Основное приложение WinAPI
add_executable(Lab2_WinAPI_Main Lab2/Main.cpp)

target_include_directories(Lab2_WinAPI_Main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Lab2
)

target_link_libraries(Lab2_WinAPI_Main PRIVATE WinAPIThreadLib)

# ---------- std::thread версия (Lab2a) ----------

# Создаем библиотеку для std::thread версии
add_library(StdThreadLib
    Lab2a/ThreadProcessing.cpp
)

target_include_directories(StdThreadLib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Lab2a
)

target_compile_features(StdThreadLib PRIVATE cxx_std_17)

# Основное приложение std::thread
add_executable(Lab2_StdThread_Main Lab2a/Main.cpp)

target_include_directories(Lab2_StdThread_Main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Lab2a
)

target_compile_features(Lab2_StdThread_Main PRIVATE cxx_std_17)
target_link_libraries(Lab2_StdThread_Main PRIVATE StdThreadLib)

# ---------- Настройка Google Test ----------

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ---------- Тесты для WinAPI версии ----------

add_executable(Lab2_WinAPI_Tests
    Test/test_winapi.cpp
    Test/pch.cpp
)

target_include_directories(Lab2_WinAPI_Tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Test
        ${CMAKE_CURRENT_SOURCE_DIR}/Lab2
)

target_link_libraries(Lab2_WinAPI_Tests PRIVATE WinAPIThreadLib gtest_main)

add_test(NAME Lab2_WinAPI_Tests COMMAND Lab2_WinAPI_Tests)

# ---------- Тесты для std::thread версии ----------

add_executable(Lab2_StdThread_Tests
    Testa/test_thread.cpp
    Testa/pch.cpp
)

target_include_directories(Lab2_StdThread_Tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Testa
        ${CMAKE_CURRENT_SOURCE_DIR}/Lab2a
)

target_compile_features(Lab2_StdThread_Tests PRIVATE cxx_std_17)
target_link_libraries(Lab2_StdThread_Tests PRIVATE StdThreadLib gtest_main)

add_test(NAME Lab2_StdThread_Tests COMMAND Lab2_StdThread_Tests)