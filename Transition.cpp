//
// Created by artem on 10.04.2020.
//

#include "Transition.h"

State* Transition::GetState(State* currentState, char symbol){
    int size = _statements.size();
    for (int i = 0; i < size; ++i) {
        if (_statements[i]->CanTransitTo(symbol)){
            return _statements[i];
        }
    }

    return nullptr;
}