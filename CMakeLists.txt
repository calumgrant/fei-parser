project (fei-parser)
cmake_minimum_required(VERSION 3.16)

enable_testing()

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(lexertests tests/testLexer.cpp 
    include/accepts.hpp include/charclasses.hpp include/feiparser.hpp include/lex.hpp include/lexer.hpp
    include/next.hpp include/node.hpp include/normalize.hpp include/output.hpp include/regex.hpp
    include/rules.hpp include/tree.hpp include/unicode.hpp
    )

add_executable(parsertests tests/testParser.cpp)
add_executable(typesetTests tests/testTypeset.cpp)

add_executable(javabench tests/javabench/javabench.cpp tests/javabench/javalexer.cpp tests/javabench/java.hpp)

add_test(NAME lexertests COMMAND lexertests)
add_test(NAME javaparse COMMAND javabench tokenize ${CMAKE_CURRENT_SOURCE_DIR}/data/AllInOne8.java)
add_test(NAME parsertests COMMAND parsertests)
add_test(NAME typesets COMMAND typesetTests)

add_executable(ugen unicode/generator.cpp)


