#ifndef RUSTCOMPILER_PARSER_H
#define RUSTCOMPILER_PARSER_H

#include <string>
#include <vector>
#include "Token.h"

class Parser {
private:
    std::vector<Token*> _tokens;
    std::vector<Token*>::iterator _currentToken;

    // Объявление переменных
    bool LetDecl();
    bool Pat();
    bool GroupLet();
    bool VarList();
    bool Init();
    bool Expr();
    bool GroupInit();
    bool LitList();
    bool Type();

    // Ветвления
    bool IfExpr();
    bool ElseTail();
    bool BoolExpr();
    bool Add();
    bool Mult();
    bool MinTerminal();
    bool IsLiteral();

    // Терминалы
    bool IsString();
    bool IsNum();
    bool IsChar();
    bool IsBool();
    bool IsID();

    // Знаки отношения
    bool IsCompOperation();

    // Вывод
    bool Println();
    bool ExprList();

    // Массивы
    bool LetArrayDecl();
    bool ArrayType();
    bool ArrayExpr();
    bool ArrayElems();

    // Блок - внутри всё что угодно
    bool Block();
    bool BlockChecker();

public:
    explicit Parser(const std::vector<Token *> &tokens);
    bool Analyze();

};

#endif //RUSTCOMPILER_PARSER_H
