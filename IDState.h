//
// Created by artem on 10.04.2020.
//

#ifndef RUSTCOMPILER_IDSTATE_H
#define RUSTCOMPILER_IDSTATE_H

#include "State.h"

class IDState : public State{
public:
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
};


#endif //RUSTCOMPILER_IDSTATE_H
