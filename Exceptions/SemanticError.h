#ifndef RUSTCOMPILER_SEMANTICERROR_H
#define RUSTCOMPILER_SEMANTICERROR_H

#include "BaseError.h"

/// @brief ошибка на этапе семантического анализа
class SemanticError : public BaseError {
public:
    explicit SemanticError(const std::string &error, const TokenLocation& location) : BaseError(error, location) {
        _error = std::string("SemanticError:\n\t") + error + "\n\tStart:\n\t\tline: "
                 + std::to_string(location.start.line) + "\n\t\tcolumn: " + std::to_string(location.start.column) + "\n\tEnd:\n\t\tline: "
                 + std::to_string(location.end.line) + "\n\t\tcolumn: " + std::to_string(location.end.column);
    }

    const char * what () const noexcept override {
        return _error.c_str();
    }
};

#endif //RUSTCOMPILER_SEMANTICERROR_H
