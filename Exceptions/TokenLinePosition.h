#ifndef RUSTCOMPILER_TOKENLINEPOSITION_H
#define RUSTCOMPILER_TOKENLINEPOSITION_H

struct TokenLinePosition {
    unsigned int line;
    unsigned int column;

    TokenLinePosition() : line(1), column(1) { }
    TokenLinePosition(const unsigned int &line, const unsigned int &column) : line(line), column(column) { }

    void AddSymbol(const char &symbol) {
        if (symbol == '\n') {
            line++;
            column = 0;
        }
        else
            column++;
    }

    friend TokenLinePosition operator - (const TokenLinePosition &left, const int &right) {
        return TokenLinePosition(left.line, left.column - 1);
    }
};

struct TokenLocation {
    TokenLinePosition start;
    TokenLinePosition end;

    TokenLocation() : start(), end() { }
    TokenLocation(const TokenLinePosition &start, const TokenLinePosition &end) : start(start), end(end) { }
    TokenLocation(const TokenLocation &tokeLocation) { start = tokeLocation.start; end = tokeLocation.end; }

    std::string ToString() {
        return std::string("Start: " + std::to_string(start.line) + " " + std::to_string(start.column)
            + " End: " + std::to_string(end.line) + " " + std::to_string(end.column));
    }
};

#endif //RUSTCOMPILER_TOKENLINEPOSITION_H
