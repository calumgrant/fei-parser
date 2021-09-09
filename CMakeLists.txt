project (fei-parser)
cmake_minimum_required(VERSION 3.16)

enable_testing()

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include external/simpletest)
add_subdirectory(parsers/java)
add_subdirectory(parsers/xml)

add_executable(lexertests tests/testLexer.cpp)

add_executable(parsertests tests/testParser.cpp)
add_executable(typesetTests tests/testTypeset.cpp)

add_executable(xmltests tests/testXML.cpp)
target_link_libraries(xmltests cellar-xml)

add_executable(javabench tests/javabench/javabench.cpp)
target_link_libraries(javabench cellar-java)

add_test(NAME lexertests COMMAND lexertests)
add_test(NAME javaparse COMMAND javabench tokenize ${CMAKE_CURRENT_SOURCE_DIR}/data/AllInOne8.java)
add_test(NAME parsertests COMMAND parsertests)
add_test(NAME typesets COMMAND typesetTests)
add_test(NAME xml COMMAND xmltests)

add_executable(ugen unicode/generator.cpp)

add_executable(calculator tests/calculator.cpp)
