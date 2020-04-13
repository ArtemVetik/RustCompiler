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

        try{
            stateMachine.TakeSymbol(sym);
        }
        catch (std::exception& error){
            std::cout << error.what() << std::endl;
            return -1;
        }
    }

    try{
        stateMachine.End();
    }
    catch (std::exception& error){
        std::cout << error.what() << std::endl;
        return -2;
    }

    stateMachine.PrintTokens();

    file.close();
    return 0;
}