#include <string>
#include <fstream>
#include "Lexer.h"


int main() {
    system("chcp 1251");
    system("cls");

    Lexer lexer("test.txt");

    try{
        lexer.Analyze();
    }
    catch (std::exception& error){
        std::cout << error.what() << std::endl;
        return 0;
    }
    
    return 0;
}