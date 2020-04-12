#ifndef RUSTCOMPILER_SEPARATORSTATE_H
#define RUSTCOMPILER_SEPARATORSTATE_H

#include "State.h"
#include <vector>

class SeparatorState : public State {
private:
    std::string _separators = "+-*/%=><!{}[];(),";
    std::vector<std::pair<std::string, TokenType>> _tokenPair;

public:
    SeparatorState();
    ~SeparatorState() override = default;
    void Init();
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    void Debug() const override {std::cout << "SeparatorState" << std::endl;}
};

#endif //RUSTCOMPILER_SEPARATORSTATE_H
