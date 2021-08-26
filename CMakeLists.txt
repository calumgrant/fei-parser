project (fei-parser)
cmake_minimum_required(VERSION 3.16)

enable_testing()
include_directories(include)

add_executable(tests tests/main.cpp)
add_test(NAME maintests COMMAND tests)

add_executable(ugen unicode/generator.cpp)

set_target_properties(tests PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF)

set_target_properties(ugen PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF)

