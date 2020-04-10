#ifndef RUSTCOMPILER_TRANSITION_H
#define RUSTCOMPILER_TRANSITION_H

#include "State.h"

class Transition {
private:
    std::vector<State*> _states;

public:
    State* GetState(const State* const &currentState, char symbol);
};

#endif //RUSTCOMPILER_TRANSITION_H
