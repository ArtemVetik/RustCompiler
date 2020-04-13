#include <string>
#include <fstream>
#include "StateMachine.h"

int main() {
    system("chcp 1251");
    system("cls");

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

    stateMachine.PrintTokens();

    file.close();
    return 0;
}