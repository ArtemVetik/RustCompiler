#include "Transition.h"

Transition::Transition() {
    _states = {new IDState(), new IgnoreState(), new NumState(), new RealNumState(), new SeparatorState(), new StringState()};
}

State* Transition::GetState(const State* const &currentState, const char symbol) {
    for (auto state : _states) {
        if (state->CanTransitTo(symbol)){
            return state;
        }
    }

    return nullptr;
}

Transition::~Transition() {

}