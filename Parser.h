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
    bool LetDecl(Token* const &token);
    bool Pat(Token* const &token);
    bool GroupLet(Token* const &token);
    bool VarList(Token* const &token);
    bool Init(Token* const &token);
    bool Expr(Token* const &token);
    bool Type(Token* const &token);

    bool BoolExpr(Token* const &token);
    bool Add(Token *const &token);
    bool Mult(Token* const &token);
    bool MinTerminal(Token* const &token);
    bool IsLiteral(Token* const &token);

    bool IsString(Token* const &token);
    bool IsNum(Token* const &token);
    bool IsChar(Token* const &token);
    bool IsBool(Token* const &token);
    bool IsID(Token* const &token);

    bool IsCompOperation(Token* const &token);
public:
    explicit Parser(const std::vector<Token *> &tokens);
    bool Analyze();

};


#endif //RUSTCOMPILER_PARSER_H
