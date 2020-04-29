#include "Transition.h"

Transition::Transition() {
    _states = {new IDState(), new IgnoreState(), new NumState(), new SeparatorState(), new StringState()};
}

State* Transition::GetState(const State* const &currentState, const char symbol) {
    std::vector<State*> tempStateList;

    for (auto state : _states) {
        if (state->CanTransitTo(symbol)) {
            tempStateList.push_back(state);
        }
    }

    if (tempStateList.empty()) return nullptr;
    if (tempStateList.size() == 1) return tempStateList.back();

    if (symbol == '.') {
        if (dynamic_cast<const IDState*>(currentState)) {
            return _states[3];
        }
        else if (dynamic_cast<const NumState*>(currentState)) {
            return _states[2];
        }
    }

    return nullptr;
}

Transition::~Transition() {
    for (auto state : _states) {
        delete state;
    }

    _states.clear();
}