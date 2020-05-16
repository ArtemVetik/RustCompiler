#ifndef RUSTCOMPILER_TOKEN_H
#define RUSTCOMPILER_TOKEN_H

#include <iostream>
#include <string>
#include <utility>
#include "TokenType.h"

class Token {
private:
    TokenType _type;
    std::string _value;
    int _id;

public:
    explicit Token(const std::string &value = "");
    Token(const TokenType &type, std::string value, int id);
    std::string GetValue() const;
    TokenType GetType() const;
    void PrintToken() const;
    //Token& operator = (const Token& token);
};

#endif //RUSTCOMPILER_TOKEN_H