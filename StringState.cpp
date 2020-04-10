#include "StringState.h"

bool StringState::Contains(const char sym) const {
    return sym == '\"';
}

bool StringState::CanTransitTo(const char sym) const {
    return Contains(sym);
}