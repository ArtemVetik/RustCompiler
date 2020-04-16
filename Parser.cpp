#include "Parser.h"

Parser::Parser(const std::vector<Token *> &tokens) : _currentToken(_currentToken) {
    _tokens = tokens;
    _currentToken = _tokens.begin();
}

bool Parser::BoolExpr(Token* const &token) {
    if (Add(*_currentToken)){
        while (IsCompOperation(*_currentToken)){
            _currentToken++;
            if (!Add(*_currentToken))
                return false;
            _currentToken++;
        }
        return true;
    }

    return false;
}

bool Parser::Add(Token *const &token) {
    return false;
}

bool Parser::Mult(Token* const &token) {
    if (MinTerminal(*_currentToken)){
        _currentToken++;

    while((*_currentToken)->GetType() == DIV || (*_currentToken)->GetType() == MULT || (*_currentToken)->GetType() == IMP){
        _currentToken++;
        if (!MinTerminal(*_currentToken))
            return false;
        _currentToken++;
    }
    return true;
}

    return false;
}

bool Parser::MinTerminal(Token* const &token) {
    return false;
}

bool Parser::IsLiteral(Token* const &token) {
    if (IsString(*_currentToken) || IsChar(*_currentToken) || IsBool(*_currentToken) || IsNum(*_currentToken)){
        _currentToken++;
        return true;
    }
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
