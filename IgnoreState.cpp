#include "IgnoreState.h"

bool IgnoreState::Contains(const char sym) const {
    return _ignoreSymbols.find(sym) != std::string::npos;
}

bool IgnoreState::CanTransitTo(const char sym) const {
    return Contains(sym);
}