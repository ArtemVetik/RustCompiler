#include "StateMachine.h"

void StateMachine::TakeSymbol(char symbol){
    if (_currentState->Contains(symbol))
        return;

    // create token
    _currentState = _transition.GetState(_currentState, symbol);
}