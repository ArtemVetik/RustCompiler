#include "Token.h"

Token::Token(const TokenType &type, std::string value) : _type(type), _value(std::move(value)) {

}

std::string Token::Value(){
    return _value;
}
TokenType Token::Type(){
    return  _type;
}
