#ifndef RUSTCOMPILER_TOKEN_H
#define RUSTCOMPILER_TOKEN_H

#include <string>
#include "TokenType.h"

class Token {
private:
    std::string _value;
    TokenType _type;

public:
    Token(const TokenType &type, const std::string &value);
};

#endif //RUSTCOMPILER_TOKEN_H
