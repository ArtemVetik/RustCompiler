#ifndef RUSTCOMPILER_CODEGENERATORERROR_H
#define RUSTCOMPILER_CODEGENERATORERROR_H

#include "BaseError.h"

/// @file CodeGeneratorError.h

/// @brief ошибка на этапе генерации кода
class CodeGeneratorError : public BaseError {
public:
    explicit CodeGeneratorError(const std::string &error) : BaseError(error, nullptr) {
        _error = std::string("CodeGenerator error:\n\t") +  error + "\n";
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_CODEGENERATORERROR_H
