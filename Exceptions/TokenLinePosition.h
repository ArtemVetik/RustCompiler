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

    friend std::ostream& operator<< (std::ostream &out, const TokenLocation &location) {
        out << "Start: " << location.start.line << " " << location.start.column
            << " End: " << location.end.line << " " << location.end.column << std::endl;
        return out;
    }
};

#endif //RUSTCOMPILER_TOKENLINEPOSITION_H
