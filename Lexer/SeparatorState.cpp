#include "SeparatorState.h"

SeparatorState::SeparatorState() : _comment(CommentType::None) {
    Init();
}

void SeparatorState::Init() {
    _tokenPair.emplace_back(std::make_pair("+", TokenType::PLUS));
    _tokenPair.emplace_back(std::make_pair("-", TokenType::MINUS));
    _tokenPair.emplace_back(std::make_pair("*", TokenType::MULT));
    _tokenPair.emplace_back(std::make_pair("/", TokenType::DIV));
    _tokenPair.emplace_back(std::make_pair("%", TokenType::MOD));
    _tokenPair.emplace_back(std::make_pair("=", TokenType::ASSIG));
    _tokenPair.emplace_back(std::make_pair("==", TokenType::EQUAL));
    _tokenPair.emplace_back(std::make_pair(">=", TokenType::MOREEQUAL));
    _tokenPair.emplace_back(std::make_pair("<=", TokenType::LESSEQUAL));
    _tokenPair.emplace_back(std::make_pair("!=", TokenType::NASSIG));
    _tokenPair.emplace_back(std::make_pair("<", TokenType::LESS));
    _tokenPair.emplace_back(std::make_pair(">", TokenType::MORE));
    _tokenPair.emplace_back(std::make_pair("{", TokenType::LFIGBR));
    _tokenPair.emplace_back(std::make_pair("}", TokenType::RFIGBR));
    _tokenPair.emplace_back(std::make_pair("[", TokenType::LSQRBR));
    _tokenPair.emplace_back(std::make_pair("]", TokenType::RSQRBR));
    _tokenPair.emplace_back(std::make_pair(":", TokenType::COLON));
    _tokenPair.emplace_back(std::make_pair(";", TokenType::SEMICOLON));
    _tokenPair.emplace_back(std::make_pair("(", TokenType::LRBR));
    _tokenPair.emplace_back(std::make_pair(")", TokenType::RRBR));
    _tokenPair.emplace_back(std::make_pair(",", TokenType::COM));
    _tokenPair.emplace_back(std::make_pair(".", TokenType::DOT));
    _tokenPair.emplace_back(std::make_pair("!", TokenType::EXCL));
    _tokenPair.emplace_back(std::make_pair("&&", TokenType::LAND));
    _tokenPair.emplace_back(std::make_pair("||", TokenType::LOR));
    _tokenPair.emplace_back(std::make_pair("&", TokenType::BAND));
    _tokenPair.emplace_back(std::make_pair("->", TokenType::ARROW));
}

Token* SeparatorState::GetToken(const std::string &value) {
    _buffer.clear();

    if (_comment != CommentType::None) {
        _comment = CommentType::None;
        return new Token(TokenType::IGNORE, value, -1);
    }

    auto foundToken = std::find_if(_tokenPair.cbegin(), _tokenPair.cend(),
    [&value](const std::pair<std::string, TokenType> &token)
    {
        return token.first == value;
    });
    int id = std::distance(_tokenPair.cbegin(), foundToken);

    if (foundToken != _tokenPair.cend())
        return new Token(foundToken->second, value, id);

    return nullptr;
}

std::vector<Token*> SeparatorState::SplitTokens(const std::string &value) {
    if (value.empty())
        return std::vector<Token*>();

    if (HasInTable(value))
        return std::vector<Token*>({GetToken(value)});

    std::vector<Token*> tokens;

    std::string subString;
    for (int j = 1; j < value.size(); ++j) {
        for (int i = 0; i <= j; ++i) {
            subString = value.substr(i, value.size()-j);
            if (HasInTable(subString)) {
                std::vector<Token*> part = SplitTokens(value.substr(0, i));
                tokens.insert(tokens.cend(), part.cbegin(), part.cend());

                tokens.emplace_back(GetToken(subString));

                part = SplitTokens(value.substr(i+subString.size(), value.size()-i-subString.size()));
                tokens.insert(tokens.cend(), part.cbegin(), part.cend());
                return tokens;
            }
        }
    }

    return std::vector<Token*>({nullptr});
}

bool SeparatorState::Contains(const char sym) {
    _buffer += sym;

    if (_buffer == "//")
        _comment = CommentType::SingleLine;
    else if (_buffer == "/*")
        _comment = CommentType::Multiline;

    if (_comment == CommentType::SingleLine)
        return sym != '\n';
    if (_comment == CommentType::Multiline)
        return !(_buffer[_buffer.size()-2] == '/' && _buffer[_buffer.size()-3] == '*');

    return _separators.find(sym) != std::string::npos;
}

bool SeparatorState::CanTransitTo(const char sym) {
    return Contains(sym);
}

bool SeparatorState::HasInTable(const std::string &value) {
    auto foundToken = std::find_if(_tokenPair.cbegin(), _tokenPair.cend(),
    [&value](const std::pair<std::string, TokenType> &token)
    {
        return token.first == value;
    });

    return foundToken != _tokenPair.cend();
}
