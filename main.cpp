#include <string>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "AST_Tree.h"

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

    Parser parser(lexer.GetTokens());
    if (!parser.Analyze())
        std::cout << "�������������� ������" << std::endl;
    else
        std::cout << "���111!!!1" << std::endl;

    return 0;
}