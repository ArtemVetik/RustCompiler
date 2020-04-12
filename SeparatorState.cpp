#include "SeparatorState.h"

SeparatorState::SeparatorState() {
    Init();
}

void SeparatorState::Init() {
    _tokenPair.emplace_back(std::make_pair("+", PLUS));
    _tokenPair.emplace_back(std::make_pair("-", MINUS));
    _tokenPair.emplace_back(std::make_pair("*", MULT));
    _tokenPair.emplace_back(std::make_pair("/", DIV));
    _tokenPair.emplace_back(std::make_pair("%", MOD));
    _tokenPair.emplace_back(std::make_pair("=", ASSIG));
    _tokenPair.emplace_back(std::make_pair("==", EQUAL));
    _tokenPair.emplace_back(std::make_pair(">=", ASMR));
    _tokenPair.emplace_back(std::make_pair("<=", ASLS));
    _tokenPair.emplace_back(std::make_pair("!=", NASSIG));
    _tokenPair.emplace_back(std::make_pair("<", LESS));
    _tokenPair.emplace_back(std::make_pair(">", MORE));
    _tokenPair.emplace_back(std::make_pair("{", LBLBR));
    _tokenPair.emplace_back(std::make_pair("}", RBLBR));
    _tokenPair.emplace_back(std::make_pair("[", SLBR));
    _tokenPair.emplace_back(std::make_pair("]", SRBR));
    _tokenPair.emplace_back(std::make_pair(":", COLON));
    _tokenPair.emplace_back(std::make_pair(";", SEMICOLON));
    _tokenPair.emplace_back(std::make_pair("(", LFBR));
    _tokenPair.emplace_back(std::make_pair(")", RGBR));
    _tokenPair.emplace_back(std::make_pair(",", COM));
    _tokenPair.emplace_back(std::make_pair(".", DOT));
    _tokenPair.emplace_back(std::make_pair("!", EXCL));
}

Token* SeparatorState::GetToken(const std::string value) {
    auto foundToken = std::find_if(_tokenPair.cbegin(), _tokenPair.cend(), [&](const std::pair<std::string, TokenType> &token) {return token.first == value;});
    if (foundToken != _tokenPair.cend())
        return new Token(foundToken->second, value);
    return nullptr;
}

bool SeparatorState::Contains(const char sym) {
    return _separators.find(sym) != std::string::npos;
}

bool SeparatorState::CanTransitTo(const char sym) {
    return Contains(sym);
}
