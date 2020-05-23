#ifndef RUSTCOMPILER_TOKEN_H
#define RUSTCOMPILER_TOKEN_H

#include <iostream>
#include <string>
#include <utility>
#include "TokenType.h"
#include "../Exceptions/TokenLinePosition.h"

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
    void SetLocation(const TokenLocation &location);
    TokenLocation* const & GetLocation() const;
    std::string GetValue() const;
    TokenType GetType() const;
    void PrintToken() const;
};

#endif //RUSTCOMPILER_TOKEN_H