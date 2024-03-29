#include "Transition.h"

Transition::Transition() {
    _states = {new IDState(), new IgnoreState(), new NumState(), new SeparatorState(), new StringState()};
}

State* Transition::GetState(State* const &currentState, const char symbol) {
    for (const auto &state : _states) {
        if (state->CanTransitTo(symbol)) {
            if (!currentState || currentState->CanTransitTo(state))
                return state;
        }
    }

    return nullptr;
}

Transition::~Transition() {
    for (auto &state : _states) {
        delete state;
    }

    _states.clear();
}