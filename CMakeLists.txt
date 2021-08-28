project (fei-parser)
cmake_minimum_required(VERSION 3.16)

enable_testing()

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(tests tests/main.cpp include/next.hpp include/normalize.hpp include/lexer.hpp include/lex.hpp)
add_executable(javabench tests/javabench/javabench.cpp tests/javabench/java.cpp)

add_test(NAME maintests COMMAND tests)
add_test(NAME javaparse COMMAND javabench ${CMAKE_CURRENT_SOURCE_DIR}/data/AllInOne8.java)

add_executable(ugen unicode/generator.cpp)

set_target_properties(tests PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF)

set_target_properties(ugen PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF)

