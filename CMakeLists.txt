cmake_minimum_required(VERSION 3.10)

project(ZeroMQController VERSION 0.1.0 LANGUAGES C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(server
    server.c
)

add_executable(client
    client.c
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

include_directories(${ZMQ_INCLUDE_DIRS})
link_directories(${ZMQ_LIBRARY_DIRS})

target_link_libraries(server ${ZMQ_LIBRARIES})
target_link_libraries(client ${ZMQ_LIBRARIES})

if (MSVC)
    target_compile_options(
        server PRIVATE
    )
    target_compile_options(
        client PRIVATE
    )
else ()
    target_compile_options(
        server PRIVATE
        # -Wall -Wextra -Wshadow -Wcast-align -Wconversion -Wsign-conversion -Wfloat-equal -Wpedantic -Werror -O2
    )
    target_compile_options(
        client PRIVATE
        # -Wall -Wextra -Wshadow -Wcast-align -Wconversion -Wsign-conversion -Wfloat-equal -Wpedantic -Werror -O2
    )
endif()