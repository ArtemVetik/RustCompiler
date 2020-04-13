#include <iostream>
#include <string>
#include <fstream>
#include "StateMachine.h"

#include <map>
using namespace std;

enum Errors {ErrorA=0, ErrorB, ErrorC};

std::ostream& operator<<(std::ostream& out, const Errors value){
    static std::map<Errors, std::string> strings;
    if (strings.size() == 0){
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(ErrorA);
        INSERT_ELEMENT(ErrorB);
        INSERT_ELEMENT(ErrorC);
#undef INSERT_ELEMENT
    }

    return out << strings[value];
}

int main() {
    system("chcp 1251");

    /*StateMachine stateMachine;
    std::ifstream file("test.txt");

    char sym;
    while (!file.eof()){
        file.get(sym);
        if (file.eof())
            break;
        stateMachine.TakeSymbol(sym);
    }
    stateMachine.End();

    file.close();*/
    std::cout << ErrorA << std::endl << ErrorB << std::endl << ErrorC << std::endl;
    return 0;

    return 0;
}