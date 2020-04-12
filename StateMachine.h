#ifndef RUSTCOMPILER_STATEMACHINE_H
#define RUSTCOMPILER_STATEMACHINE_H

#include "State.h"
#include "Transition.h"
#include <string>
#include <cstdlib>

class StateMachine {
private:
    State *_currentState;
    Transition _transition;
    std::string _buffer;

public:
    StateMachine();
    void TakeSymbol(char symbol);
    void End();
};


#endif //RUSTCOMPILER_STATEMACHINE_H
