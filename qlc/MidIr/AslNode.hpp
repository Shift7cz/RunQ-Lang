#pragma once

enum AslNodeType {
    Unassigned,
    Root,

    // functions
    Func,
    ExternFunc,
    Return,

    // control flow
    Marker,
    Jump,
    CondJump,

    // data + literals
    MakeImutInt,
    MakeImutFloat,
    MakeImutChar,
    MakeImutBool,
    CreateMut,
    StoreMut,
    CallFn,

    // ops
    MathInstr,
    CompareInstr,
};