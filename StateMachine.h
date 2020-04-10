#ifndef RUSTCOMPILER_STATEMACHINE_H
#define RUSTCOMPILER_STATEMACHINE_H

#include "State.h"
#include "Transition.h"

class StateMachine {
private:
    State *_currentState;
    Transition _transition;

public:
    void TakeSymbol(char symbol);
};


#endif //RUSTCOMPILER_STATEMACHINE_H
