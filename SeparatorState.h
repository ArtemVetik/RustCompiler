#ifndef RUSTCOMPILER_SEPARATORSTATE_H
#define RUSTCOMPILER_SEPARATORSTATE_H

#include "State.h"

class SeparatorState : public State {
private:
    std::string _separators = "+-*/%=><{}[];(),.";

public:
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
    void Debug() const override {std::cout << "SeparatorState" << std::endl;}
};

#endif //RUSTCOMPILER_SEPARATORSTATE_H
