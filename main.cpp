#include <string>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantics/Table.h"
#include "Semantics/TableData.h"
#include "Semantics/SemanticAnalyzer.h"
#include "CodeGenerator/CodeGenerator.h"

/************************************************************************************
* Работу выполнили студенты И584 Труфанов Глеб Евгеньевич и Ветик Артём Владимирович*
* @Военмех - лучше всех                                                             *
* RUST: 1.42.0                                                                      *
* 03.05.2020                                                                        *
*************************************************************************************
*/

std::string Menu() {
    char menu;

    do {
        system("cls");
        std::cout << "Выберите файл на Rust'е:" << std::endl;
        std::cout << "1. Квадратное уравнение" << std::endl;
        std::cout << "2. Бинарная сортировка" << std::endl;
        std::cout << "3. Алгоритм проверки на простое число" << std::endl;
        std::cout << "4. Выход" << std::endl;

        menu = getchar();
        while(getchar() != '\n');

        switch (menu) {
            case '1':
                return "test.txt";
            case '2':
                return "test2.txt";
            case '3':
                return "test3.txt";
            default:
                break;
        }
    } while(menu != '4');

    return "";
}

int main() {
    system("chcp 1251");
    system("cls");

    return 0;
    /*std::string fileName = Menu();

    if (fileName.empty()) {
        system("pause");
        return 0;
    }*/

    try {
        Lexer lexer("test.txt");
        lexer.Analyze();
        std::cout << "Лексический анализ прошёл успешно..." << std::endl;

        Parser parser(lexer.GetTokens());
        parser.Analyze();
        std::cout << "Синтаксический анализ прошёл успешно..." << std::endl;

        SemanticAnalyzer semanticAnalyzer(parser.GetASTTree());
        semanticAnalyzer.Analyze();
        std::cout << "Семантический анализ прошёл успешно..." << std::endl;

        CodeGenerator generator(parser.GetASTTree(), semanticAnalyzer.GetFunctionTable());
        generator.Generate();
    }
    catch (std::exception& error) {
        std::cout << error.what() << std::endl;
        system("pause");
        return 0;
    }




    system("pause");
    return 0;
}