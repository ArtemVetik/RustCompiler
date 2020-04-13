#ifndef RUSTCOMPILER_LEXERROR_H
#define RUSTCOMPILER_LEXERROR_H

#include <iostream>
#include <exception>
#include <string>

class LexError : public std::exception {
private:
    std::string _error;

public:
    explicit LexError(std::string &error) {
        _error = std::string("LexError:\n\t") + error;
    }
    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_LEXERROR_H
