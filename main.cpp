#include <string>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "AST_Tree.h"

 /************************************************************************************
 * Работу выполнили студенты И584 Труфанов Глеб Евгеньевич и Ветик Артём Владимирович*
 * @Военмех - лучше всех                                                             *
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

    std::string fileName = Menu();

    if (fileName.empty()) {
        system("pause");
        return 0;
    }

    Lexer lexer(fileName);

    try {
        lexer.Analyze();
        std::cout << "Лексический анализ прошёл успешно..." << std::endl;
    }
    catch (std::exception& error){
        std::cout << error.what() << std::endl;
        return 0;
    }

    Parser parser(lexer.GetTokens());
    try {
        parser.Analyze();
        std::cout << "Синтаксический анализ прошёл успешно..." << std::endl;
    }
    catch (std::exception& error) {
        std::cout << error.what() << std::endl;
    }

    std::cout << "\n";
    system("pause");
    return 0;
}