#ifndef RUSTCOMPILER_STRINGSTATE_H
#define RUSTCOMPILER_STRINGSTATE_H

#include "State.h"
#include <string>

/// @file StringState.h

/// @brief Состояние строки
class StringState : public State {
private:
    bool _isActive;

public:
    StringState();
    ~StringState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    Token* GetToken(const std::string &value) override;
};

#endif //RUSTCOMPILER_STRINGSTATE_H
