#include <iostream>
#include <string>
#include <fstream>
#include "StateMachine.h"

int main() {
    StateMachine stateMachine;

    std::ifstream file("test.txt");

    char sym;
    while (!file.eof()) {
        file >> sym;
        stateMachine.TakeSymbol(sym);
        if (file.eof()) break;
    }

    file.close();
    return 0;
}