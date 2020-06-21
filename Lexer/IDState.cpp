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
        return new Token(TokenType::ID, value, id);
    }
}

void IDState::InitKeyWords() {
    _keyWords.emplace_back(std::make_pair("fn", TokenType::FUNCTION));
    _keyWords.emplace_back(std::make_pair("for", TokenType::FOR));
    _keyWords.emplace_back(std::make_pair("if", TokenType::IF));
    _keyWords.emplace_back(std::make_pair("loop", TokenType::LOOP));
    _keyWords.emplace_back(std::make_pair("while", TokenType::WHILE));
    _keyWords.emplace_back(std::make_pair("else", TokenType::ELSE));
    _keyWords.emplace_back(std::make_pair("let", TokenType::LET));
    _keyWords.emplace_back(std::make_pair("mut", TokenType::MUT));
    _keyWords.emplace_back(std::make_pair("f32", TokenType::REAL));
    _keyWords.emplace_back(std::make_pair("i32", TokenType::INTEGER));
    _keyWords.emplace_back(std::make_pair("usize", TokenType::UINT));
    _keyWords.emplace_back(std::make_pair("println", TokenType::PRINTLN));
    _keyWords.emplace_back(std::make_pair("print", TokenType::PRINT));
    _keyWords.emplace_back(std::make_pair("break", TokenType::BREAK));
    _keyWords.emplace_back(std::make_pair("return", TokenType::RETURN));
    _keyWords.emplace_back(std::make_pair("true", TokenType::BOOL));
    _keyWords.emplace_back(std::make_pair("false", TokenType::BOOL));
}

bool IDState::CanTransitTo(State *const &to) const {
    return dynamic_cast<NumState*>(to) == nullptr;
}