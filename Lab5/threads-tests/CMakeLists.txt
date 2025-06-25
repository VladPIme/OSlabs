include(FetchContent)

# Скачивание Catch2 из GitHub
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# Создание исполняемого файла тестов
add_executable(threads_tests
    src/tests.cpp
)

# Линковка с Catch2 и библиотеками потоков (используем библиотеки, а не exe)
target_link_libraries(threads_tests PRIVATE
    Catch2::Catch2WithMain
    threads_server_lib
    threads_client_lib
)

# Добавление флага для компиляции с поддержкой потоков в Linux
if(NOT WIN32)
    target_compile_options(threads_tests PUBLIC -pthread)
    target_link_options(threads_tests PUBLIC -pthread)
endif()

# Регистрация тестов
include(CTest)
include(Catch)
catch_discover_tests(threads_tests)