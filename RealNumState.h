#ifndef RUSTCOMPILER_REALNUMSTATE_H
#define RUSTCOMPILER_REALNUMSTATE_H

#include "State.h"

class RealNumState : public State {
public:
    ~RealNumState() override = default;
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
    void Debug() const override {std::cout << "RealNumState" << std::endl;}
};

#endif //RUSTCOMPILER_REALNUMSTATE_H
