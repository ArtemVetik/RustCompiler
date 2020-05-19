#ifndef RUSTCOMPILER_SEMANTICERROR_H
#define RUSTCOMPILER_SEMANTICERROR_H

#include <iostream>
#include <exception>
#include <string>

class SemanticError : public std::exception {
private:
    std::string _error;

public:
    explicit SemanticError(const std::string &error) {
        _error = std::string("SemanticError:\n\t") + error;
    }
    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_SEMANTICERROR_H
