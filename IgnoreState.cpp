#include "IgnoreState.h"

bool IgnoreState::Contains(const char sym) {
    return _ignoreSymbols.find(sym) != std::string::npos;
}

bool IgnoreState::CanTransitTo(const char sym) {
    return Contains(sym);
}

Token* IgnoreState::GetToken(const std::string value){
    return new Token(IGNORE, "", -1);
}