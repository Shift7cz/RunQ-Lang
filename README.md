# RunQ-Lang

RunQ-Lang is an experimental, statically-typed programming language, inspired by C++ and Rust. The project is written in C++ and translates directly to LLVM IR which compiles it down to x86 assembly, and an executable file.

> **Project Status:** Under active development. Language features, AST design, and internal implementations are evolving rapidly.

---

## Features & Implementation Overview

* **Custom Lexer & Tokenizer:** Zero-copy string view tokenization with support for positions, identifiers, literals, and operators.
* **Recursive-Descent Parser:** Converts source tokens into a strongly typed Abstract Syntax Tree (AST).
* **LLVM Code Generator:** Custom C++ LLVM wrapper layer (`LlvmWrap`) generating clean, optimizable LLVM IR for target functions, variables, and math operations and can compile code to actual executables.
* **Primitive Types:** Supports `i32` integer variables, basic arithmetic operations, and simple functions.

---

## Code Example

```runq
fn main() : i32 {
    
    let i32 x = 12;
    let i32 y = 64;
    let i8 z = 144;
    let f64 pi = 3.141592;
    let char letter = 'q';
    let bool isSomething = true;
    
    if x >= y { 
        return x + 27 + 6;
    }
    else {
        return 27 + x + x;
    }
}
```

---

## Roadmap/Planed Features


| Potential future features (not garanteed) | Planned features  | Currently in development | Already implemented |
| --------- | --------- | --------- | --------- |
| C FFI | Loops (while, for, loop all rust style) | Bool operations | Basic math functions |
| Package manager (using GitHub) | Multi function code | Literal typing (internal only, doesnt change syntax) | Variables loading and declaring |
| Multiple file compatibility | Strings, arrays, vectors | Reading from file | Return statement |
| Multi Threading | OOP (C++ style) |  |  i32, i8, f64, bool and char data types |
| Bootstrapping | LLVM Compiler optimilastions |  | Compileation to assembly and executible using LLVM |
|  | More data types (i64, u64, u32, u8, f32, f128) |  | Basic if/else (accepts variables and comapre expresions) |
|  | Different return types |  |  |
|  | Longer math expresions working properly |  |  |


---

## Contribution

RunQ-Lang is currently under active development. Contributions, bug reports, and suggestions are welcome!

Feel free to open an issue or submit a Pull Request if you'd like to help improve the project.

---

> **Note:** RunQ-Lang is a personal project developed alongside my studies. Development happens in bursts as time permits, so progress may be non-linear.