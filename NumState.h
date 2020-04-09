//
// Created by artem on 10.04.2020.
//

#ifndef RUSTCOMPILER_NUMSTATE_H
#define RUSTCOMPILER_NUMSTATE_H

#include "State.h"


class NumState : public  State{
public:
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
};


#endif //RUSTCOMPILER_NUMSTATE_H
