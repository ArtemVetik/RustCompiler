#include "SeparatorState.h"

SeparatorState::SeparatorState() {
    Init();
}

void SeparatorState::Init() {
    _tokenPair.emplace_back(std::make_pair("+", PLUS));
    _tokenPair.emplace_back(std::make_pair("-", MINUS));
    _tokenPair.emplace_back(std::make_pair("+=", PLUSEQUAL));
    _tokenPair.emplace_back(std::make_pair("-=", MINUSEQUAL));
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
    _tokenPair.emplace_back(std::make_pair("&&", LAND));
    _tokenPair.emplace_back(std::make_pair("||", LOR));
    _tokenPair.emplace_back(std::make_pair("&", BAND));
    _tokenPair.emplace_back(std::make_pair("->", ARROW));
}

Token* SeparatorState::GetToken(const std::string &value) {
    auto foundToken = std::find_if(_tokenPair.cbegin(), _tokenPair.cend(), [&value](const std::pair<std::string, TokenType> &token) {return token.first == value;});
    int id = std::distance(_tokenPair.cbegin(), foundToken);

    if (foundToken != _tokenPair.cend())
        return new Token(foundToken->second, value, id);

    return nullptr;
}

std::vector<Token*> SeparatorState::SplitTokens(const std::string &value){
    std::vector<Token*> tokens;
    for (char i : value) {
        tokens.emplace_back(GetToken(std::string(1, i)));
    }

    return tokens;
}

bool SeparatorState::Contains(const char sym) {
    return _separators.find(sym) != std::string::npos;
}

bool SeparatorState::CanTransitTo(const char sym) {
    return Contains(sym);
}