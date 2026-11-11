#pragma once
#include <cstddef>

enum TypeKind {
    Int,
    Float,
    Bool,
    Char,
    Void,

    Unknown // error
};

struct Type {
    TypeKind kind;
    size_t bitSize;
    bool isSigned;
};