project (fei-parser)
cmake_minimum_required(VERSION 3.16)

enable_testing()

add_executable(main main.cpp)
add_test(NAME main COMMAND main)

set_target_properties(main PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF)
