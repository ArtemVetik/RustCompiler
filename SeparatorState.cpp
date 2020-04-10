#include "SeparatorState.h"

bool SeparatorState::Contains(const char sym) const {
    return _separators.find(sym) != std::string::npos;
}

bool SeparatorState::CanTransitTo(const char sym) const {
    return Contains(sym);
}
