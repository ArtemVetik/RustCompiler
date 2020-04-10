#ifndef RUSTCOMPILER_STRINGSTATE_H
#define RUSTCOMPILER_STRINGSTATE_H

#include "State.h"

class StringState : public State{
    bool Contains(char sym) const override;
    bool CanTransitTo(char sym) const override;
};


#endif //RUSTCOMPILER_STRINGSTATE_H
