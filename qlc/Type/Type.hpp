#pragma once
#include <cstddef>

enum TypeKind {
    Int_,
    Float_,
    Bool_,
    Char_,
    Void_,

    Unknown_ // error
};

struct Type {
    TypeKind kind;
    size_t bitSize;
    bool isSigned;
};