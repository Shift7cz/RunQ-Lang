# RunQ-Lang

RunQ-Lang is an experimental, truing complete, statically-typed programming language, inspired by C++ and Rust. The project is written in C++ and translates directly to LLVM IR which compiles it down to x86 assembly, and an executable file.

> **Project Status:** Under active development. Language features, AST design, and internal implementations are evolving rapidly.

---

[//]: # (## Features & Implementation Overview)

[//]: # ()
[//]: # (* **Custom Lexer & Tokenizer:** Zero-copy string view tokenization with support for positions, identifiers, literals, and operators.)

[//]: # (* **Recursive-Descent Parser:** Converts source tokens into a strongly typed Abstract Syntax Tree &#40;AST&#41;.)

[//]: # (* **LLVM Code Generator:** Custom C++ LLVM wrapper layer &#40;`LlvmWrap`&#41; generating clean, optimizable LLVM IR for target functions, variables, and math operations and can compile code to actual executables.)

[//]: # (* **Primitive Types:** Supports `i32` integer variables, basic arithmetic operations, and simple functions.)

[//]: # ()
[//]: # (---)

## Code Example

```runq
fn main() : i32 { // this is a comment

    let i32 x = 12; // declare immutable variable
    let~ i32 y = 2 * x;
    y = y - 4; // mod variable

    let f64 pi = 3.141592;
    let~ f64 otherDouble = pi; // declare mutable variable
    otherDouble = 123.4567;

    let i8 z = 58i8; // these will not be in LLVM IR because of compiler optimisations
    let i8 w = z + 9i8;

    let char letter = 'q';
    let bool isSomething = true;

    let~ i32 returnValue = 0;
    if x >= y { // If statements
        returnValue = x + 27 + 6 + 4; // math
    }
    else {
        returnValue = 27 + x + y;
    }

    let~ i32 i = 0;
    while i < 20 { // while loop
        i = i + 1
    }

    return returnValue + i; // return
}
```

---

## Roadmap/Planed Features


| Potential future features (not garanteed) | Planned features  | Currently in development | Already implemented |
| --------- | --------- | --------- | --------- |
| C FFI | Autmomatic typing in some situations based on context | Bool operations | Basic math functions |
| Package manager (using GitHub) | Multi function code | Different return types (return statement typing)  | Variables loading and declaring |
| Multiple file compatibility | Strings, arrays, vectors | Loops (for, loop all rust style) | Return statement |
| Multi Threading | OOP (C++ style) | Math overhoul (Longer math expresions working properly) |  i32, i8, f64, bool and char data types |
| Bootstrapping | LLVM Compiler optimilastions |  | Compileation to assembly and executible using LLVM |
|  | More data types (i64, u64, u32, u8, f32, f128) |  | Basic if/else (accepts variables and comapre expresions) |
|  |  |  | Reading from file |
|  |  |  | Mutable/Imutable variables (let/let~) |   
| | | | Comments|
| | | | While loop |


---

## Contribution

RunQ-Lang is currently under active development. Contributions, bug reports, and suggestions are welcome!

Feel free to open an issue or submit a Pull Request if you'd like to help improve the project.

---

> **Note:** RunQ-Lang is a personal project developed alongside my studies. Development happens in bursts as time permits, so progress may be non-linear.