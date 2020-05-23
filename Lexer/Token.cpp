#include "Token.h"

Token::Token(const TokenType &type, std::string value, const int id) : _location(nullptr), _type(type), _value(std::move(value)), _id(id) { }

std::string Token::GetValue() const {
    return _value;
}

TokenType Token::GetType() const {
    return _type;
}

void Token::PrintToken() const {
    std::cout << "[TypeData] " << TokenTypePrinter::PrintTokenType(_type) << "\n" << "[Value] " << _value << "\n" <<
              "[ID] " << _id << "\nLocation: " << (_location ? _location->ToString() : "none") << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}

Token::Token(const std::string &value) : _location(nullptr) {
    _type = TokenType::IGNORE;
    _value = value;
    _id = 0;
}

void Token::SetLocation(const TokenLocation &location) {
    if (_location) {
        delete _location;
        _location = nullptr;
    }

    _location = new TokenLocation(location);
}

TokenLocation* const &Token::GetLocation() const {
    return _location;
}

Token::~Token() {
    if (_location) {
        delete _location;
        _location = nullptr;
    }
}

Token &Token::operator=(const Token &right) {
    if (_location) {
        delete _location;
        _location = nullptr;
    }
    if (right._location)
        _location = new TokenLocation(right._location->start, right._location->end);

    _type = right._type;
    _value = right._value;
    _id = right._id;

    return *this;
}
