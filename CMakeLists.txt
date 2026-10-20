cmake_minimum_required(VERSION 4.2)
project(RunQ-Lang)

set(CMAKE_CXX_STANDARD 23)

set(LLVM_DIR "/usr/lib/llvm/21/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# links core llvm libs
llvm_map_components_to_libnames(llvm_libs
        core
        support
        irreader
        codegen
        mc
        mcparser
        object
        target
        x86codegen
        x86asmparser
        x86desc
        x86info
)

add_executable(RunQ-Lang qlc/main.cpp
        qlc/Lexer/Lexer.cpp
        qlc/Lexer/Lexer.hpp
        qlc/Parser/Parser.cpp
        qlc/Parser/Parser.hpp
        qlc/Checker/Checker.cpp
        qlc/Checker/Checker.hpp
        qlc/Codegen/CodeGen.cpp
        qlc/Codegen/CodeGen.hpp
        qlc/Lexer/Token.hpp
        qlc/Parser/Node.hpp
        qlc/Parser/Ast.hpp
        qlc/Parser/Ast.cpp
        qlc/Codegen/LlvmWrap.cpp
        qlc/Codegen/LlvmWrap.hpp
        qlc/Project/Project.cpp
        qlc/Project/Project.hpp
        qlc/Checker/Logger.cpp
        qlc/Checker/Logger.hpp
)

find_library(LLVM_SHARED_LIB LLVM HINTS ${LLVM_LIBRARY_DIRS})

target_link_libraries(RunQ-Lang PRIVATE
        ${LLVM_SHARED_LIB}
)