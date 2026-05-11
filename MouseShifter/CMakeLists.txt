cmake_minimum_required(VERSION 3.15)
project(MouseShifter VERSION 1.0 LANGUAGES CXX)

# Стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для Windows - настройки компиляции
if(WIN32)
    # Добавляем определения для Windows API
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/MouseShifter.cpp
)

# Заголовочные файлы
set(HEADERS
    include/MouseShifter.h
)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключаем директорию с заголовками
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Для Windows подключаем необходимые библиотеки
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32 gdi32)
endif()

# Установка выходной директории
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Информация о проекте
message(STATUS "=== Mouse Shifter Project ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
