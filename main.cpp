#include <string>
#include <fstream>
#include "StateMachine.h"

int main() {
    system("chcp 1251");
    system("cls");

    StateMachine stateMachine;
    std::ifstream file("test2.txt");

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
            return 0;
        }
    }

    try{
        stateMachine.End();
    }
    catch (std::exception& error){
        std::cout << error.what() << std::endl;
        return 0;
    }

    stateMachine.PrintTokens();

    file.close();
    return 0;
}