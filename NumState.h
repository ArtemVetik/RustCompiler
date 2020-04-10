#ifndef RUSTCOMPILER_NUMSTATE_H
#define RUSTCOMPILER_NUMSTATE_H

#include "State.h"

class NumState : public State {
public:
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
};

#endif //RUSTCOMPILER_NUMSTATE_H
