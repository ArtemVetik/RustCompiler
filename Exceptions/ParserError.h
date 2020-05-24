#ifndef RUSTCOMPILER_PARSERERROR_H
#define RUSTCOMPILER_PARSERERROR_H

#include <exception>
#include <string>
#include "TokenLinePosition.h"
#include "../Lexer/Token.h"

class ParserError : public std::exception {
private:
    std::string _error;

public:
    ParserError() {
        _error = "nil";
    }

    explicit ParserError(const char &expectedSymbol, const Token* token) {
        if (token == nullptr)
            _error = std::string("Expected ") + expectedSymbol + ", found nil";
        else
            _error = std::string("ParserError:\n\tExpected ") + expectedSymbol + " symbol, found " + token->GetValue() + "\n\tStart:\n\t\tline: "
                 + std::to_string(token->GetLocation()->start.line) + "\n\t\tcolomn: " + std::to_string(token->GetLocation()->start.column) + "\n\tEnd:\n\t\tline: "
                 + std::to_string(token->GetLocation()->end.line) + "\n\t\tcolomn: " + std::to_string(token->GetLocation()->end.column);
    }
    explicit ParserError(const std::string &expectedExpression, const Token* token) {
        if (token == nullptr)
            _error = std::string("Expected ") + expectedExpression + ", found nil";
        else
            _error = std::string("ParserError:\n\tExpected ") + expectedExpression + ", found " + token->GetValue() + "\n\tStart:\n\t\tline: "
                 + std::to_string(token->GetLocation()->start.line) + "\n\t\tcolomn: " + std::to_string(token->GetLocation()->start.column) + "\n\tEnd:\n\t\tline: "
                 + std::to_string(token->GetLocation()->end.line) + "\n\t\tcolomn: " + std::to_string(token->GetLocation()->end.column);
    }
    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_PARSERERROR_H
