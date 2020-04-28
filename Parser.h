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
    bool Type();

    bool BoolExpr();
    bool Add();
    bool Mult();
    bool MinTerminal();
    bool IsLiteral();

    bool IsString();
    bool IsNum();
    bool IsChar();
    bool IsBool();
    bool IsID();

    bool IsCompOperation();
public:
    explicit Parser(const std::vector<Token *> &tokens);
    bool Analyze();

};


#endif //RUSTCOMPILER_PARSER_H
