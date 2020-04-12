#include "NumState.h"

bool NumState::Contains(const char sym) {
    return (sym >= '0' && sym <= '9') || sym == '.';
}



bool NumState::CanTransitTo(const char sym) {
    return Contains(sym);
}

Token* NumState::GetToken(const std::string value) {
    int pointCount = std::count(value.cbegin(), value.cend(), '.');
    switch(pointCount) {
        case 0 :
            return new Token(INTEGER, value);
        case 1 :
            return new Token(REAL, value);
        default:
            return nullptr;
    }
}
