# RunQ-Lang

RunQ-Lang is an experimental, Turing complete, statically-typed programming language, inspired by C++ and Rust. The project is written in C++ and translates directly to LLVM IR which compiles it down to any assembly supported by LLVM, and an executable file.

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
fn isPositive(i32 num) : bool {
    return num > 0;
}

fn calculate(i32 a, i32 b) : i32 { // function
    let~ i32 result = a;
    let~ i32 i = 0; // mutable var

    while i < b { // loop
        result = result + a;
        i = i + 1;
    }

    return result;
}

entry main() { // code starts here
    let i32 x = calculate(3, 4);
    let bool check = isPositive(x);
    let i8 small = 10i8; // immutable var
    let f64 pi = 3.14;

    if check { // if statement
        return x;
    } else {
        return 0;
    }
}
```

---

## Roadmap/Planed Features


| Potential future features (not garanteed) | Planned features  | Currently in development | Already implemented |
| --------- | --------- | --------- | --------- |
| Bootstrapping | Autmomatic typing in some situations based on context | Types overhoul (internal) | Basic math functions |
| Package manager (using GitHub) | Loops (for, loop all rust style) | Basic Checker | Variables loading and declaring |
| Multi Threading | Strings, arrays, vectors | Library inport | Return statement |
|  | OOP (C++ style) |  |  i32, i8, f64, bool and char data types |
|  | LLVM Compiler optimilastions |  | Compileation to assembly and executible using LLVM |
|  | More data types (i64, u64, u32, u8, f32, f128) |  | Basic if/else (accepts variables and comapre expresions) |
|  | Bool operations  |  | Reading from file |
|  | Basic stdlib |  | Mutable/Imutable variables (let/let~) |   
|  | Full Checker |  | Comments|
|  |  |  | While loop |
|  |  |  | Multi functional code with arguments and function calling |
|  |  |  | C FFI | 
|  |  |  | Proper project structure (multiple files, proper bin folder) |
|  |  |  | Two pass codegen (functions can be in whatever order) |
|  |  |  | Proper error handeling and logging |

---

## Contribution

RunQ-Lang is currently under active development. Contributions, bug reports, and suggestions are welcome!

Feel free to open an issue or submit a Pull Request if you'd like to help improve the project.

---

> **Note:** RunQ-Lang is a personal project developed alongside my studies. Development happens in bursts as time permits, so progress may be non-linear.