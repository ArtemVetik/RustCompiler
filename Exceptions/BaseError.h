#ifndef RUSTCOMPILER_BASEERROR_H
#define RUSTCOMPILER_BASEERROR_H

#include <string>
#include <exception>
#include "TokenLinePosition.h"
#include "../Lexer/Token.h"

/// @file BaseError.h

/// @brief базовый класс ошибок компилятора
class BaseError : public std::exception{
protected:
    std::string _error;
    TokenLocation _location;

public:
    explicit BaseError(const std::string &error, Token* const &token) {
        if (token == nullptr)
            _location = TokenLocation();
        else
            _location = *token->GetLocation();
        _error = error;
    }

    explicit BaseError(const std::string &error, const TokenLocation &location) {
        _location = location;
        _error = error;
    }

    /// @brief возвращает позицию токена
    const TokenLocation& GetTokenLocation() const {
        return _location;
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};


#endif //RUSTCOMPILER_BASEERROR_H
