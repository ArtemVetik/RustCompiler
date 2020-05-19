#ifndef RUSTCOMPILER_PARSERERROR_H
#define RUSTCOMPILER_PARSERERROR_H

#include <exception>
#include <string>

class ParserError : public std::exception {
private:
    std::string _error;

public:
    explicit ParserError(const std::string &error) {
        _error = std::string("ParserError:\n\t") + error;
    }
    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_PARSERERROR_H
