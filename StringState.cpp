#include "StringState.h"

StringState::StringState(){
    _isActive = false;
}

bool StringState::Contains(const char sym) {
    bool returnVal = _isActive;
    if (sym == '\"')
        _isActive = false;
    return returnVal;
}

bool StringState::CanTransitTo(const char sym) {
    return  _isActive = (sym == '\"');
}