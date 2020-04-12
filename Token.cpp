#include "Token.h"

Token::Token(const TokenType &type, std::string value) : _type(type), _value(std::move(value)) {

}
