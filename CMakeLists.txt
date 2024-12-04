cmake_minimum_required(VERSION 3.10)

# Информация об авторе
set(CMLIB_AUTHOR_NAME "Погожев Максим" CACHE STRING "")
set(CMLIB_AUTHOR_EMAIL "crazyblackfire@mail.ru" CACHE STRING "")
set(CMLIB_DESCRIPTION "ОС, лабороторная работа номер 5-7, вариант 17" CACHE STRING "")

# Настройки проекта
project(test VERSION 0.1.0 LANGUAGES C)

# Исходные файлы
set(
    CONTROLLER_SOURCES
    managing_node.c
)

set(
    WORKER_SOURCES
    computing_node.c
)

# Добавление исполняемых файлов
add_executable(
    controller
    ${CONTROLLER_SOURCES}
)

add_executable(
    computing_node
    ${WORKER_SOURCES}
)

# Поиск и линковка библиотеки ZeroMQ
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

include_directories(${ZMQ_INCLUDE_DIRS})
link_directories(${ZMQ_LIBRARY_DIRS})

target_link_libraries(controller ${ZMQ_LIBRARIES})
target_link_libraries(computing_node ${ZMQ_LIBRARIES})

# Настройки компиляции для MSVC и других компиляторов
if (MSVC)
    target_compile_options(
        controller PRIVATE
        # /W4
    )
    target_compile_options(
        computing_node PRIVATE
        # /W4
    )
else ()
    target_compile_options(
        controller PRIVATE
        # -Wall
        # -Wextra
        # -Wshadow
        # -Wcast-align
        # -Wconversion
        # -Wsign-conversion
        # -Wfloat-equal
        # -Wpedantic
        # -Werror
        # -pedantic
        # -O2
    )
    target_compile_options(
        computing_node PRIVATE
        # -Wall
        # -Wextra
        # -Wshadow
        # -Wcast-align
        # -Wconversion
        # -Wsign-conversion
        # -Wfloat-equal
        # -Wpedantic
        # -Werror
        # -pedantic
        # -O2
    )
endif()