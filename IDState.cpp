//
// Created by artem on 10.04.2020.
//

#include "IDState.h"

bool IDState::Contains(char sym){
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z') || (sym >= '1' && sym <= '9');
}

bool IDState::CanTransitTo(char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z');
}