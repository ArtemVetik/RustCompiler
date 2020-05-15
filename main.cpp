#include <string>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "AST_Tree.h"

 /************************************************************************************
 * ������ ��������� �������� �584 �������� ���� ���������� � ����� ���� ������������*
 * @������� - ����� ����                                                             *
 * 03.05.2020                                                                        *
 *************************************************************************************
 */

std::string Menu() {
    char menu;

    do {
        system("cls");
        std::cout << "�������� ���� �� Rust'�:" << std::endl;
        std::cout << "1. ���������� ���������" << std::endl;
        std::cout << "2. �������� ����������" << std::endl;
        std::cout << "3. �������� �������� �� ������� �����" << std::endl;
        std::cout << "4. �����" << std::endl;

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
        std::cout << "����������� ������ ������ �������..." << std::endl;
    }
    catch (std::exception& error){
        std::cout << error.what() << std::endl;
        return 0;
    }

    Parser parser(lexer.GetTokens());
    try {
        parser.Analyze();
        std::cout << "�������������� ������ ������ �������..." << std::endl;
    }
    catch (std::exception& error) {
        std::cout << error.what() << std::endl;
    }

    std::cout << "\n";
    system("pause");
    return 0;
}