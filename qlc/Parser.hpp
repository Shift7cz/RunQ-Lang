#pragma once
#include <vector>

#include "Node.hpp"

class Parser {
public:
    std::vector<Node> ast;

    void parse();
};
