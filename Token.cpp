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
    std::cout << "[Type] " << AAAPrintTokenType(_type) << "\n" << "[Value] " << _value << "\n" <<
                                                                                        "[ID] " << _id << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}
