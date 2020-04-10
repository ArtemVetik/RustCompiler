#ifndef RUSTCOMPILER_REALNUMSTATE_H
#define RUSTCOMPILER_REALNUMSTATE_H

#include "State.h"

class RealNumState : public State {
public:
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
};

#endif //RUSTCOMPILER_REALNUMSTATE_H
