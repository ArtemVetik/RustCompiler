#ifndef RUSTCOMPILER_TOKENLINEPOSITION_H
#define RUSTCOMPILER_TOKENLINEPOSITION_H

/// @file TokenLinePosition.h

/// @brief позиция символа в файле
struct TokenLinePosition {
    unsigned int line; ///< @brief строка
    unsigned int column; ///< @brief столбец
    unsigned int linearPosition; ///< @brief линейная позиция

    TokenLinePosition() : line(1), column(1), linearPosition(0) { }
    TokenLinePosition(const unsigned int &line, const unsigned int &column, const unsigned int &linearPosition)
        : line(line), column(column), linearPosition(linearPosition) { }

    /// @brief меняет позицию строки и столбца по входящему символу
    void AddSymbol(const char &symbol) {
        linearPosition++;
        if (symbol == '\n') {
            line++;
            column = 0;
        }
        else
            column++;
    }

    friend TokenLinePosition operator + (const TokenLinePosition &left, const std::string &right) {
        TokenLinePosition newPosition(left);
        for (const auto &sym : right)
            newPosition.AddSymbol(sym);
        return newPosition;
    }

    friend TokenLinePosition operator - (const TokenLinePosition &left, const int &right) {
        return TokenLinePosition(left.line, left.column - 1, left.linearPosition - 1);
    }
};

/// @brief позиция токена
struct TokenLocation {
    TokenLinePosition start; ///< @brief позиция первого символа
    TokenLinePosition end; ///< @brief позиция последнего символа

    TokenLocation() : start(), end() { }
    TokenLocation(const TokenLinePosition &start, const TokenLinePosition &end) : start(start), end(end) { }
    TokenLocation(const TokenLocation &tokeLocation) { start = tokeLocation.start; end = tokeLocation.end; }

    std::string ToString() {
        return std::string("Start: " + std::to_string(start.line) + " " + std::to_string(start.column)
            + " End: " + std::to_string(end.line) + " " + std::to_string(end.column));
    }
};

#endif //RUSTCOMPILER_TOKENLINEPOSITION_H
