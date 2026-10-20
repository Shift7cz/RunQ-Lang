#include "Project.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

enum ProjectTokenType {
    Runq,
    C,
    Library,
    Identifier,
    Accesor, // >

    EndOfFile,
};

struct ProjectToken {
    ProjectTokenType Type;
    std::string Data;
};

void Project::load(std::string path) {
    // load
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string runqProject = buffer.str();

    //std::cout << runqProject << std::endl;

    // lexer
    std::vector<ProjectToken> tokens;
    std::string current;
    for (int i = 0; i < runqProject.length(); i++) {
        if (runqProject[i] == '\n' || runqProject[i] == ' ') {
            if (current == "[RunQ]") {
                tokens.push_back(ProjectToken{Runq, current}); current = "";
            }
            else if (current == "[C]") {
                tokens.push_back(ProjectToken{C, current}); current = "";
            }
            else if (current == "[Library]") {
                tokens.push_back(ProjectToken{Library, current}); current = "";
            }
            else if (current == ">") {
                tokens.push_back(ProjectToken{Accesor, current}); current = "";
            }
            else if (runqProject[i] == '\0') {
                tokens.push_back(ProjectToken{EndOfFile, ""}); current = "";
            }
            else {
                tokens.push_back(ProjectToken{Identifier, current}); current = "";
            }
        }
        else current += runqProject[i];

        //std::cout << current << std::endl;
    }

    // parser
    ProjectTokenType group;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].Type == Runq) {
            group = Runq;
        }
        else if (tokens[i].Type == C) {
            group = C;
        }
        else if (tokens[i].Type == Library) {
            group = Library;
        }
        else if (tokens[i].Type == Accesor && tokens[i+1].Type == Identifier) {
            i++;
            switch (group) {
                case Runq:
                    runqList.push_back(tokens[i].Data);
                    break;
                case C:
                    cList.push_back(tokens[i].Data);
                    break;
                case Library:
                    libList.push_back(tokens[i].Data);
            }
        }
    }

    // debug
    // for (int i = 0; i < cList.size(); i++) {
    //     std::cout << cList[i] << std::endl;
    // }
    // for (int i = 0; i < runqList.size(); i++) {
    //     std::cout << runqList[i] << std::endl;
    // }
}
