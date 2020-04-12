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

    std::cout << _buffer << std::endl << "------------------------" << std::endl;
    _buffer = "";
    _buffer += symbol;

    _currentState = _transition.GetState(_currentState, symbol);
}

void StateMachine::End(){
    std::cout << _buffer << std::endl << "------------------------" << std::endl;
    _buffer = "";
}