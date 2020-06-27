#ifndef RUSTCOMPILER_NUMSTATE_H
#define RUSTCOMPILER_NUMSTATE_H

#include "State.h"

/// @brief состояние числа
/// @details число может быть как целым, так и вещественным. Разделение происходит на этапе формированиия токена
class NumState : public State {
public:
    ~NumState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    Token* GetToken(const std::string &value) override;
};

#endif //RUSTCOMPILER_NUMSTATE_H
