# Добавление библиотек для сервера и клиента
add_library(threads_server_lib STATIC
    src/server.cpp
)

add_library(threads_client_lib STATIC
    src/client.cpp
)

# Указание директорий включения для библиотек
target_include_directories(threads_server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(threads_client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Создание исполняемых файлов
add_executable(threads_server_exe
    src/server_main.cpp
)

add_executable(threads_client_exe
    src/client_main.cpp
)

# Линковка библиотек
target_link_libraries(threads_server_exe PRIVATE 
    threads_server_lib
    threads_client_lib
)

# Добавляем линковку с библиотекой сервера для клиента
target_link_libraries(threads_client_exe PRIVATE 
    threads_client_lib
    threads_server_lib
)

# Добавление флага для компиляции с поддержкой потоков
# В Windows потоки доступны без специальных флагов
if(NOT WIN32)
    target_compile_options(threads_server_lib PUBLIC -pthread)
    target_compile_options(threads_client_lib PUBLIC -pthread)
    target_link_options(threads_server_exe PUBLIC -pthread)
    target_link_options(threads_client_exe PUBLIC -pthread)
endif()