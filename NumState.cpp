#include "NumState.h"

bool NumState::Contains(const char sym) {
    return (sym >= '0' && sym <= '9') || sym == '.';
}

bool NumState::CanTransitTo(const char sym) {
    return Contains(sym);
}