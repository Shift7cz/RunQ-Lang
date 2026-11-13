#include "Asl.hpp"
#include "../Checker/Logger.hpp"

void Asl::print() {
    if (list.size() == 0) Log::log("Nothing to print; asl is empty!!!");

    for (int i = 0; i < list.size(); ++i) {
        printNode(i);
    }
}

void Asl::printNode(int index) {
    switch (list[index].type) {
        case Unassigned:
            Log::log("Unassigned node!!!");
            break;
        case Func:
            Log::log("Func: ");
            break;
        case ExternFunc:
            Log::log("ExternFunc: ");
            break;
        case Ret:
            Log::log("Ret: ");
            break;
        case Marker:
            Log::log("Marker: ");
            break;
        case Jump:
            Log::log("Jump: ");
            break;
        case CondJump:
            Log::log("CondJump: ");
            break;
        case MakeImut:
            Log::log("MakeImut: ");
            break;
        case CreateMut:
            Log::log("CreateMut: ");
            break;
        case StoreMut:
            Log::log("StoreMut: ");
            break;
        case CallFn:
            Log::log("CallFn: ");
            break;
        case MathInstr:
            Log::log("MathInstr: ");
            break;
        case CompareInstr:
            Log::log("CompareInstr: ");
            break;
        default:
            Log::log("Unknown node type!!! Check AslNode.hpp!!!");
    }
}
