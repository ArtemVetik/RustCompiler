#include "SeparatorState.h"

bool SeparatorState::Contains(const char sym) {
    return _separators.find(sym) != std::string::npos;
}

bool SeparatorState::CanTransitTo(const char sym) {
    return Contains(sym);
}
