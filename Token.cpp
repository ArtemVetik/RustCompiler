#include "Token.h"

Token::Token(const TokenType &type, std::string value, const int id) : _type(type), _value(std::move(value)), _id(id) {

}

std::string Token::GetValue() const {
    return _value;
}
TokenType Token::GetType() const {
    return _type;
}

void Token::PrintToken() const {
    std::cout << AAAPrintTokenType(_type) << "\t" << _value << "\t" << _id << std::endl;
    std::cout << std::string(20,'-') << std::endl;
}
