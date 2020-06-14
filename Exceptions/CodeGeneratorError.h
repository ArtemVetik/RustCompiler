#ifndef RUSTCOMPILER_CODEGENERATORERROR_H
#define RUSTCOMPILER_CODEGENERATORERROR_H

#include <iostream>
#include <exception>
#include <string>

class CodeGeneratorError : public std::exception {
private:
    std::string _error;

public:
    explicit CodeGeneratorError(const std::string &error) {
        _error = std::string("LexError:\n\t") +  error + "\n";
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_CODEGENERATORERROR_H
