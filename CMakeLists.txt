project (fei-parser)
cmake_minimum_required(VERSION 3.16)

enable_testing()

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_CXX_FLAGS "-ftemplate-depth=2048")

include_directories(include external/simpletest)
add_subdirectory(parsers/java)
add_subdirectory(parsers/xml)
add_subdirectory(parsers/json)

add_executable(lexertests tests/testLexer.cpp)

add_executable(parsertests tests/testParser.cpp tests/testParser2.cpp)
add_executable(typesetTests tests/testTypeset.cpp)

add_executable(xmltests tests/testXML.cpp)
target_link_libraries(xmltests cellar-xml)

add_executable(javabench tests/java/javabench.cpp)
target_link_libraries(javabench cellar-java)

add_executable(jsontests tests/testJson.cpp)
target_link_libraries(jsontests cellar-json)

add_executable(treetest tests/treetest.cpp tests/typeset_test.hpp)
add_executable(listtest tests/listtest.cpp tests/typeset_test.hpp)

add_executable(compilation1 tests/compilation/compilation1.cpp tests/compilation/grammar.hpp)
target_link_libraries(compilation1 cellar-java)
add_executable(compilation2 tests/compilation/compilation2.cpp tests/compilation/grammar.hpp)
add_executable(compilation3 tests/compilation/compilation3.cpp tests/compilation/grammar.hpp)
add_executable(compilation4 tests/compilation/compilation4.cpp tests/compilation/grammar.hpp)


add_test(NAME lexertests COMMAND lexertests)
add_test(NAME javaparse COMMAND javabench tokenize ${CMAKE_CURRENT_SOURCE_DIR}/data/AllInOne8.java)
add_test(NAME parsertests COMMAND parsertests)
add_test(NAME typesets COMMAND typesetTests)
add_test(NAME typetree COMMAND treetest)
add_test(NAME typelist COMMAND treetest)
add_test(NAME xml COMMAND xmltests)
add_test(NAME json COMMAND jsontests)
add_test(NAME compilation1 COMMAND compilation1)
add_test(NAME compilation2 COMMAND compilation2)
add_test(NAME compilation3 COMMAND compilation3)
add_test(NAME compilation4 COMMAND compilation4)

add_executable(ugen unicode/generator.cpp)

add_executable(calculator tests/calculator.cpp)
