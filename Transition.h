//
// Created by artem on 10.04.2020.
//

#ifndef RUSTCOMPILER_TRANSITION_H
#define RUSTCOMPILER_TRANSITION_H

#include "State.h"

class Transition {
private:
    std::vector<State*> _statements;

public:
    State* GetState(State* currentState, char symbol);
};


#endif //RUSTCOMPILER_TRANSITION_H
