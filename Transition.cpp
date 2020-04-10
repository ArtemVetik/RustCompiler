#include "Transition.h"

Transition::Transition(){
    _states = {};
}

State* Transition::GetState(const State* const &currentState, const char symbol){
    for (auto state : _states) {
        if (state->CanTransitTo(symbol)){
            return state;
        }
    }

    return nullptr;
}