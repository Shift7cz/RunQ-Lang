#pragma once
#include <vector>
#include "AslNode.hpp"

class Asl {
public:
    std::vector<AslNode> list;

    void print();

private:
    void printNode(int index);
};
