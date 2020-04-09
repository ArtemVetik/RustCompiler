//
// Created by artem on 10.04.2020.
//

#ifndef RUSTCOMPILER_STATE_H
#define RUSTCOMPILER_STATE_H

#include <vector>
#include <iostream>
#include <algorithm>

class State {
public:
    virtual bool Contains(char sym) = 0;
    virtual bool CanTransitTo(char sym) = 0;
};


#endif //RUSTCOMPILER_STATE_H
