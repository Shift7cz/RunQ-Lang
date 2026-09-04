# RunQ-Lang

RunQ-Lang is an experimental, statically-typed programming language written in C++ that compiles directly to LLVM IR.

> **Project Status:** Under active development. Language features, AST design, and internal implementations are evolving rapidly.

---

## Features & Implementation Overview

* **Custom Lexer & Tokenizer:** Zero-copy string view tokenization with support for positions, identifiers, literals, and operators.
* **Recursive-Descent Parser:** Converts source tokens into a strongly typed Abstract Syntax Tree (AST).
* **LLVM Code Generator:** Custom C++ LLVM wrapper layer (`LlvmWrap`) generating clean, optimizable LLVM IR for target functions, variables, and math operations.
* **Primitive Types:** Supports `i32` integer variables, basic arithmetic operations, and simple functions.

---

## Code Example

```runq
fn main() : i32 {
    let i32 x = 12;
    return 27 * x;
}
```

---

## Roadmap/Planed Features


| Potential future features | Planned features  | Already implemented |
| --------- | --------- | --------- |
| C FFI | Control flow | Basic math functions |
| Package manager (using GitHub) | Multi function code | Variables loading and declaring |
| Multiple file compatibility | More data types (char, bool, i64, u64, i32, i8, u8, f64) | Return statement |
|  | Strings, arrays, vectors | i32 Data type |
|  | OOP (C++ style) |  |
|  | Loops (while, for, loop all rust style) |  |

---

## Contribution

RunQ-Lang is currently under active development. Contributions, bug reports, and suggestions are welcome!

Feel free to open an issue or submit a Pull Request if you'd like to help improve the project.

---

> **Note:** RunQ-Lang is a personal project developed alongside my studies. Development happens in bursts as time permits, so progress may be non-linear.