cmake_minimum_required(VERSION 4.2)
project(RunQ-Lang)

set(CMAKE_CXX_STANDARD 23)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# links core llvm libs
llvm_map_components_to_libnames(llvm_libs core support native)

add_executable(RunQ-Lang qlc/main.cpp
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
        qlc/Ast.hpp
        qlc/Ast.cpp
)

target_link_libraries(RunQ-Lang PRIVATE ${llvm_libs})