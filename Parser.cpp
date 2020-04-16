#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) : _currentToken(_currentToken) {
    _tokens = tokens;
    _currentToken = _tokens.begin();
}

bool Parser::BoolExpr(Token* const &token) {
    return false;
}

bool Parser::SimpleBoolExpr(Token* const &token) {
    return false;
}

bool Parser::Mult(Token* const &token) {
    return false;
}

bool Parser::MinTerminal(Token* const &token) {
    return false;
}

bool Parser::IsLiteral(Token* const &token) {
    return false;
}

bool Parser::IsString(Token* const &token) {
    return false;
}

bool Parser::IsNum(Token* const &token) {
    return false;
}

bool Parser::IsChar(Token* const &token) {
    return false;
}

bool Parser::IsBool(Token* const &token) {
        return false;
}

bool Parser::IsID(Token* const &token) {
    return false;
}

bool Parser::IsCompOperation(Token* const &token) {
    return false;
}

bool Parser::Analyze() {
    return BoolExpr(*_currentToken);
}
