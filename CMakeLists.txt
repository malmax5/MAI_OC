cmake_minimum_required(VERSION 3.10)

set(CMLIB_AUTHOR_NAME "Погожев Максим" CACHE STRING "")
set(CMLIB_AUTHOR_EMAIL "crazyblackfire@mail.ru" CACHE STRING "")
set(CMLIB_DESCRIPTION "ООП, лабороторная работа номер 2, вариант 4" CACHE STRING "")

project(test VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(
    SOURCES
    main.cpp
    src/Complex.cpp
    src/Matrix.cpp
    src/firstSolution.cpp
    src/secondSolution.cpp
    includes/Complex.hpp
    includes/Matrix.hpp
)

add_executable(
    lab_2
    ${SOURCES}
)

if (MSVC)

    target_compile_options(
        lab_2 PRIVATE
        /W4
    )
else ()
    target_compile_options(
        lab_2 PRIVATE
        # -Wall
        # -Wextra
        # -Wshadow
        # -Wcast-align
        # -Wfloat-equal
        # -Wpedantic
        # -Werror
        # -pedantic
        # -O2
    )
endif()