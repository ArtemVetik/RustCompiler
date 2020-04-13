#include <utility>

//
// Created by artem on 13.04.2020.
//

#ifndef RUSTCOMPILER_LEXERROR_H
#define RUSTCOMPILER_LEXERROR_H


#include <iostream>
#include <exception>
#include <string>

class LexError : public std::exception {
private:
    char* _error;

public:
    LexError(const char* error) {

    }
    const char * what () const noexcept override {
        return _error;
    }
};


#endif //RUSTCOMPILER_LEXERROR_H
