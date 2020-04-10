#ifndef RUSTCOMPILER_STRINGSTATE_H
#define RUSTCOMPILER_STRINGSTATE_H

#include "State.h"
#include <string>

class StringState : public State{

public:
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
    void Debug() const override {std::cout << "StringState" << std::endl;}
};

#endif //RUSTCOMPILER_STRINGSTATE_H
