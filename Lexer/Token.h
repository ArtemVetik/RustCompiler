#ifndef RUSTCOMPILER_TOKEN_H
#define RUSTCOMPILER_TOKEN_H

#include <iostream>
#include <string>
#include <utility>
#include "TokenType.h"
#include "../Exceptions/TokenLinePosition.h"

/// @file Token.h

/// @brief Класс, описывабщий токен
class Token {
private:
    TokenLocation* _location;
    TokenType _type;
    std::string _value;
    int _id;

public:
    explicit Token(const std::string &value = "");
    Token(const TokenType &type, std::string value, int id);
    ~Token();
    /// @brief Задает позицию токена в тексте программы
    /// @param location позиция токена
    void SetLocation(const TokenLocation &location);
    /// @return возвращает позицию токена
    [[nodiscard]] TokenLocation* const & GetLocation() const;
    [[nodiscard]] std::string GetValue() const;
    [[nodiscard]] TokenType GetType() const;
    void PrintToken() const;

    Token& operator = (const Token &right);
};

#endif //RUSTCOMPILER_TOKEN_H