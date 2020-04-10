#include <iostream>
#include <string>
#include <fstream>
#include "StateMachine.h"

int main() {
    StateMachine stateMachine;

    std::ifstream file("test.txt");

    char sym;
    while (!file.eof()){
        file.get(sym);
        if (file.eof())
            break;
        stateMachine.TakeSymbol(sym);
    }
    stateMachine.End();

    file.close();
    return 0;
}