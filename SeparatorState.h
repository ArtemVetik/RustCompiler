#ifndef RUSTCOMPILER_SEPARATORSTATE_H
#define RUSTCOMPILER_SEPARATORSTATE_H

#include "State.h"

class SeparatorState : public State {
private:
    std::string _separators = "+-*/%=><{}[];(),";

public:
    ~SeparatorState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    void Debug() const override {std::cout << "SeparatorState" << std::endl;}
};

#endif //RUSTCOMPILER_SEPARATORSTATE_H
