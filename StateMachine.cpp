#include "StateMachine.h"

StateMachine::StateMachine() {
    _buffer = "";
    _currentState = nullptr;
}

void StateMachine::TakeSymbol(char symbol) {
    if (_currentState && _currentState->Contains(symbol)){
        _buffer += symbol;
        return;
    }

    // ... create token

    if (_currentState) _currentState->Debug();
    std::cout << _buffer<< std::endl;

    _currentState = _transition.GetState(_currentState, symbol);
    _buffer = "";
    std::cout << _currentState << std::endl;
}