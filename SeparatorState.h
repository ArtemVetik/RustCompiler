#ifndef RUSTCOMPILER_SEPARATORSTATE_H
#define RUSTCOMPILER_SEPARATORSTATE_H

#include "State.h"
#include <map>

class SeparatorState : public State {
private:
    std::string _separators = "+-*/%=><{}[];(),";
public:
    ~SeparatorState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    TokenType* GetTokenType(const std::string value) override{};
};

#endif //RUSTCOMPILER_SEPARATORSTATE_H
