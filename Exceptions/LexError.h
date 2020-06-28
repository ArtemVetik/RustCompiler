#ifndef RUSTCOMPILER_LEXERROR_H
#define RUSTCOMPILER_LEXERROR_H

#include "BaseError.h"

/// @file LexError.h

/// @brief ошибка на этапе лексического анализа
class LexError : public BaseError {
public:
    explicit LexError(const std::string &error, const TokenLocation &location) : BaseError(error, location) {
        _error = std::string("LexError:\n\t") + error + "\n\tStart:\n\t\tline: "
                + std::to_string(location.start.line) + "\n\t\tcolumn: " + std::to_string(location.start.column) + "\n\tEnd:\n\t\tline: "
                + std::to_string(location.end.line) + "\n\t\tcolumn: " + std::to_string(location.end.column);
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_LEXERROR_H
