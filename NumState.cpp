//
// Created by artem on 10.04.2020.
//

#include "NumState.h"

bool NumState::Contains(char sym){
    return sym >= '0' && sym <= '9';
}

bool NumState::CanTransitTo(char sym) {
    return Contains(sym);
}