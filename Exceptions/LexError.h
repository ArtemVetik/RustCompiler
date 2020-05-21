#ifndef RUSTCOMPILER_LEXERROR_H
#define RUSTCOMPILER_LEXERROR_H

#include <iostream>
#include <exception>
#include <string>
#include "TokenLinePosition.h"

class LexError : public std::exception {
private:
    std::string _error;

public:
    explicit LexError(const std::string &error, const TokenLocation &location) {
        _error = std::string("LexError:\n\t") + error + "\n\tStart:\n\t\tline: "
                + std::to_string(location.start.line) + "\n\t\tcolomn: " + std::to_string(location.start.column) + "\n\tEnd:\n\t\tline: "
                + std::to_string(location.end.line) + "\n\t\tcolomn: " + std::to_string(location.end.column);
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_LEXERROR_H
