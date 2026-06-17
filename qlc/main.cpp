#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"

int main() {
    std::cout << "Hello, RunQ!" << std::endl;

    std::string source = "fn main( ) { \n"
                         "  return 67;\n"
                         "} ";

    std::cout << "Using test source (RunQ project is in active development) ->\n\n" << source << "\n\nLexer out ->"<< std::endl;

    Lexer lexer(source);

    bool isAtEnd = false;
    while (!isAtEnd) {
        Token token = lexer.nextToken();
        std::cout << token << std::endl;
        if (token.type == TokenType::EndOfFile) {
            isAtEnd = true;
        }
    }

    std::cout << "\n\nParser out ->" << std::endl;

    Lexer lexer2(source);
    Parser parser(lexer2);

    parser.parse();
    parser.ast.print();

    return 0;
}
