#pragma once
#include <cstddef>

enum TypeKind {
    Int,
    Float,
    Bool,
    Char,
    Void
};

struct Type {
    TypeKind kind;
    size_t bitSize;
    bool isSigned;
};