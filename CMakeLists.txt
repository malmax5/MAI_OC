cmake_minimum_required(VERSION 3.10)
project(DynamicLibraries)

# Устанавливаем стандарт C++11
set(CMAKE_CXX_STANDARD 11)

# Добавляем библиотеки
add_library(prime_count_eratosthenes SHARED Lib/src/Func_1/prime_count_eratosthenes.cpp)
add_library(prime_count_naive SHARED Lib/src/Func_1/prime_count_naive.cpp)
add_library(e_formule SHARED Lib/src/Func_2/e_formule.cpp)
add_library(e_series SHARED Lib/src/Func_2/e_series.cpp)

# Добавляем исполняемые файлы
add_executable(main_compile_time Main1/main1.cpp)
target_link_libraries(main_compile_time prime_count_eratosthenes e_formule)

add_executable(main_runtime Main2/main2.cpp)