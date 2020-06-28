#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Semantics/Table.h"
#include "Semantics/TableData.h"
#include "Semantics/SemanticAnalyzer.h"
#include "CodeGenerator/CodeGenerator.h"

/************************************************************************************
* ������ ��������� �������� �584 �������� ���� ���������� � ����� ���� ������������*
* @������� - ����� ����                                                             *
* RUST: 1.42.0                                                                      *
* 03.05.2020                                                                        *
*************************************************************************************
*/

int main() {
    system("chcp 1251");
    system("cls");

    //FILE* file = freopen("Output/consoleOutput.txt","w",stdout);

    try {
        Lexer lexer("test.txt");
        lexer.Analyze();
        std::cout << "����������� ������ ������ �������..." << std::endl;

        Parser parser(lexer.GetTokens());
        parser.Analyze();
        std::cout << "�������������� ������ ������ �������..." << std::endl;

        SemanticAnalyzer semanticAnalyzer(parser.GetASTTree());
        semanticAnalyzer.Analyze();
        std::cout << "������������� ������ ������ �������..." << std::endl;

        CodeGenerator generator(parser.GetASTTree(), semanticAnalyzer.GetFunctionTable());
        generator.Generate();
        generator.CreateAsmFile();
        //fclose(file);
        //remove("Output/errorOutput.txt");
    }
    catch (const std::exception& error) {
        //fclose(file);
        //file = freopen("Output/errorOutput.txt","w",stdout);
        std::cout << error.what() << std::endl;
        //fclose(file);
    }

    return 0;
}
