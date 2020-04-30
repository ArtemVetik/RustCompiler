#ifndef RUSTCOMPILER_TRANSITION_H
#define RUSTCOMPILER_TRANSITION_H

#include "State.h"
#include "IDState.h"
#include "NumState.h"
#include "IgnoreState.h"
#include "StringState.h"
#include "SeparatorState.h"
#include <vector>

class Transition {
private:
    std::vector<State*> _states;

public:
    Transition();
    ~Transition();

    State* GetState(State* const &currentState, char symbol);
};

#endif //RUSTCOMPILER_TRANSITION_H
