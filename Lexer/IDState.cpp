#include "IDState.h"
#include "NumState.h"

IDState::IDState(){
    InitKeyWords();
}

bool IDState::Contains(const char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z') || (sym >= '1' && sym <= '9');
}

bool IDState::CanTransitTo(const char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z');
}

Token* IDState::GetToken(const std::string &value) {
    auto keyToken = std::find_if(_keyWords.cbegin(), _keyWords.cend(),
    [&value](const std::pair<std::string, TokenType> &token)
    {
        return token.first == value;
    });
    auto idToken = std::find(_idTable.cbegin(), _idTable.cend(), value);

    if (keyToken != _keyWords.cend()) {
        int id = std::distance(_keyWords.cbegin(), keyToken);
        return new Token(keyToken->second, value, id);
    }
    else {
        if (idToken == _idTable.cend()) {
            _idTable.emplace_back(value);
            idToken = _idTable.cend() - 1;
        }
        int id = std::distance(_idTable.cbegin(), idToken);
        return new Token(ID, value, id);
    }
}

void IDState::InitKeyWords() {
    _keyWords.emplace_back(std::make_pair("fn", FUNCTION));
    _keyWords.emplace_back(std::make_pair("for", FOR));
    _keyWords.emplace_back(std::make_pair("if", IF));
    _keyWords.emplace_back(std::make_pair("loop", LOOP));
    _keyWords.emplace_back(std::make_pair("while", WHILE));
    _keyWords.emplace_back(std::make_pair("else", ELSE));
    _keyWords.emplace_back(std::make_pair("let", LET));
    _keyWords.emplace_back(std::make_pair("mut", MUT));
    _keyWords.emplace_back(std::make_pair("f32", REAL));
    _keyWords.emplace_back(std::make_pair("i32", INTEGER));
    _keyWords.emplace_back(std::make_pair("usize", UINT));
    _keyWords.emplace_back(std::make_pair("println", PRINTLN));
    _keyWords.emplace_back(std::make_pair("print", PRINT));
    _keyWords.emplace_back(std::make_pair("break", BREAK));
    _keyWords.emplace_back(std::make_pair("return", RETURN));
    _keyWords.emplace_back(std::make_pair("true", BOOL));
    _keyWords.emplace_back(std::make_pair("false", BOOL));
}

bool IDState::CanTransitTo(State *const &to) const {
    return dynamic_cast<NumState*>(to) == nullptr;
}