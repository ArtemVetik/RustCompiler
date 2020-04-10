#ifndef RUSTCOMPILER_IDSTATE_H
#define RUSTCOMPILER_IDSTATE_H

#include "State.h"

class IDState : public State{
public:
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
};

#endif //RUSTCOMPILER_IDSTATE_H
