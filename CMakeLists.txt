cmake_minimum_required(VERSION 3.10)

set(CMLIB_AUTHOR_NAME "Погожев Максим" CACHE STRING "")
set(CMLIB_AUTHOR_EMAIL "crazyblackfire@mail.ru" CACHE STRING "")
set(CMLIB_DESCRIPTION "ОС, лабороторная работа номер 2, вариант 4" CACHE STRING "")

project(test VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(includes)
# include_directories(src)

set(
    SOURCES_PARENT
    includes/exceptions/exceptions.hpp
    src/exceptions/exceptions.cpp
    src/parent.cpp
)

set(
    SOURCES_CHILD
    includes/exceptions/exceptions.hpp
    src/exceptions/exceptions.cpp
    src/child.cpp
    # src/parent.cpp
)

add_executable(
    parent
    ${SOURCES_PARENT}
)

add_executable(
    child
    ${SOURCES_CHILD}
)

# if (MSVC)
#     target_compile_options(
#         lab_1 PRIVATE
#         /W4
#     )
# else ()
#     target_compile_options(
#         lab_1 PRIVATE
#         # -Wall
#         # -Wextra
#         # -Wshadow
#         # -Wcast-align
#         # -Wconversion
#         # -Wsign-conversion
#         # -Wfloat-equal
#         # -Wpedantic
#         # -Werror
#         # -pedantic
#         # -O2
#     )
# endif()
