cmake_minimum_required(VERSION 4.2)
project(RunQ)

set(CMAKE_CXX_STANDARD 23)

add_executable(RunQ qlc/main.cpp
        qlc/Lexer.cpp
        qlc/Lexer.hpp
        qlc/Parser.cpp
        qlc/Parser.hpp
        qlc/Checker.cpp
        qlc/Checker.hpp
        qlc/CodeGen.cpp
        qlc/CodeGen.hpp
        qlc/Token.hpp
        qlc/Node.hpp
)
