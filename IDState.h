#ifndef RUSTCOMPILER_IDSTATE_H
#define RUSTCOMPILER_IDSTATE_H

#include "State.h"

class IDState : public State{
public:
    ~IDState() override = default;
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
    void Debug() const override {std::cout << "IDState" << std::endl;}
};

#endif //RUSTCOMPILER_IDSTATE_H
