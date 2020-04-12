#include "IgnoreState.h"

bool IgnoreState::Contains(const char sym) {
    return _ignoreSymbols.find(sym) != std::string::npos;
}

bool IgnoreState::CanTransitTo(const char sym) {
    return Contains(sym);
}

TokenType* IgnoreState::GetTokenType(const std::string value){
    return nullptr;
}