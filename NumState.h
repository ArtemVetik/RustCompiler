//
// Created by artem on 10.04.2020.
//

#ifndef RUSTCOMPILER_NUMSTATE_H
#define RUSTCOMPILER_NUMSTATE_H

#include "State.h"


class NumState : public  State{
public:
    NumState(){
        _symbols = {'1'};
    }

    bool CanTransitTo(char sym) override{
        return std::find(_symbols.begin(), _symbols.end(), sym) != _symbols.end();
    }
};


#endif //RUSTCOMPILER_NUMSTATE_H
