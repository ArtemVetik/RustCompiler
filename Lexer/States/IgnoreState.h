#ifndef RUSTCOMPILER_IGNORESTATE_H
#define RUSTCOMPILER_IGNORESTATE_H

#include "State.h"
#include <string>

/// @file IgnoreState.h

/// @brief Состояние игнорируемых символов
/// @details Содержит символы \\n \\t \\r space
class IgnoreState : public State {
private:
    std::string _ignoreSymbols = " \n\r\t";

public:
    ~IgnoreState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    Token* GetToken(const std::string &value) override;
};

#endif //RUSTCOMPILER_IGNORESTATE_H
