#include <string>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"

int main() {
    system("chcp 1251");
    system("cls");

    Lexer lexer("test3");

    try{
        lexer.Analyze();
    }
    catch (std::exception& error){
        std::cout << error.what() << std::endl;
        return 0;
    }

    Parser parser(lexer.GetTokens());
    if (!parser.Analyze())
        std::cout << "Синтаксическая ошибка" << std::endl;
    else
        std::cout << "Ура111!!!1" << std::endl;

    return 0;
}