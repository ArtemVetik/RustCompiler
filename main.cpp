#include <iostream>
#include <string>
#include <fstream>
#include "StateMachine.h"

int main() {
    StateMachine stateMachine;

    std::ifstream file("test.txt");

    char sym;
    while (1){
        stateMachine.TakeSymbol(sym);
    }

    file.close();
    return 0;
}