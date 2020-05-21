#include "Token.h"

Token::Token(const TokenType &type, std::string value, const int id) : _location(), _type(type), _value(std::move(value)), _id(id) { }

std::string Token::GetValue() const {
    return _value;
}

TokenType Token::GetType() const {
    return _type;
}

void Token::PrintToken() const {
    std::cout << "[TypeData] " << TokenTypePrinter::PrintTokenType(_type) << "\n" << "[Value] " << _value << "\n" <<
              "[ID] " << _id << "\nLocation: " << _location << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}

Token::Token(const std::string &value) : _location() {
    _type = TokenType::IGNORE;
    _value = value;
    _id = 0;
}

void Token::SetLocation(const TokenLocation &location) {
    _location = location;
}

const TokenLocation &Token::GetLocation() const {
    return _location;
}
