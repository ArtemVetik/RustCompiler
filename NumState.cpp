#include "NumState.h"

bool NumState::Contains(const char sym) const {
    return sym >= '0' && sym <= '9';
}

bool NumState::CanTransitTo(const char sym) const {
    return Contains(sym);
}