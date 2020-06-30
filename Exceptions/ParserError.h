#ifndef RUSTCOMPILER_PARSERERROR_H
#define RUSTCOMPILER_PARSERERROR_H

#include "BaseError.h"

/// @file ParserError.h

/// @brief ошибка на этапе синтаксического анализа
class ParserError : public BaseError {
public:
    ParserError() : BaseError("", TokenLocation()) {
        _error = "Parser Error\n\t\tunknown";
    }

    explicit ParserError(const char &expectedSymbol, Token* const &token, TokenLocation lastSymbolLocation = {}) : BaseError(std::string(1, expectedSymbol), token) {
        if (token == nullptr) {
            _error = std::string("ParserError:\n\tExpected ") + expectedSymbol + ", found end of file";
            _location = lastSymbolLocation;
        }
        else
            _error = std::string("ParserError:\n\tExpected ") + expectedSymbol + " symbol, found " + token->GetValue() + "\n\tStart:\n\t\tline: "
                 + std::to_string(token->GetLocation()->start.line) + "\n\t\tcolumn: " + std::to_string(token->GetLocation()->start.column) + "\n\tEnd:\n\t\tline: "
                 + std::to_string(token->GetLocation()->end.line) + "\n\t\tcolumn: " + std::to_string(token->GetLocation()->end.column);
    }

    explicit ParserError(const std::string &expectedExpression, Token* const &token, TokenLocation lastSymbolLocation = {}) : BaseError(expectedExpression, token) {
        if (token == nullptr) {
            _error = std::string("ParserError:\n\tExpected ") + expectedExpression + ", found end of file";
            _location = lastSymbolLocation;
        }
        else
            _error = std::string("ParserError:\n\tExpected ") + expectedExpression + ", found " + token->GetValue() + "\n\tStart:\n\t\tline: "
                 + std::to_string(token->GetLocation()->start.line) + "\n\t\tcolumn: " + std::to_string(token->GetLocation()->start.column) + "\n\tEnd:\n\t\tline: "
                 + std::to_string(token->GetLocation()->end.line) + "\n\t\tcolumn: " + std::to_string(token->GetLocation()->end.column);
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_PARSERERROR_H
