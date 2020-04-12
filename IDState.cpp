#include "IDState.h"

IDState::IDState(){
    InitKeyWords();
}

bool IDState::Contains(const char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z') || (sym >= '1' && sym <= '9');
}

bool IDState::CanTransitTo(const char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z');
}

Token* IDState::GetToken(const std::string value) {
    auto keyToken = std::find_if(_keyWords.cbegin(), _keyWords.cend(), [&](const std::pair<std::string, TokenType> &token) {return token.first == value;});
    auto idToken = std::find(_idTable.cbegin(), _idTable.cend(), value);

    if (keyToken != _keyWords.cend()) {
        return new Token(keyToken->second, value);
    }
    else {
        if (idToken == _idTable.cend())
            _idTable.emplace_back(value);
        return new Token(ID, value);
    }
}

void IDState::InitKeyWords() {
    _keyWords.emplace_back(std::make_pair("for", FOR));
    _keyWords.emplace_back(std::make_pair("if", IF));
    _keyWords.emplace_back("else", ELSE);
    _keyWords.emplace_back("let", LET);
    _keyWords.emplace_back("f32", REAL);
    _keyWords.emplace_back("i32", INTEGER);
    _keyWords.emplace_back("println", PRINTLN);
}