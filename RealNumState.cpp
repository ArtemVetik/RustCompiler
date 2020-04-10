#include "RealNumState.h"

bool RealNumState::Contains(const char sym) const {
    return (sym >= '0' && sym <= '9') || sym == '.';
}

bool RealNumState::CanTransitTo(const char sym) const {
    return Contains(sym);
}