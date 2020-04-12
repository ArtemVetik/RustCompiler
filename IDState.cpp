#include "IDState.h"

bool IDState::Contains(const char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z') || (sym >= '1' && sym <= '9');
}

bool IDState::CanTransitTo(const char sym) {
    return sym == '_' || (sym >= 'a' && sym <= 'z') || (sym >= 'A' && sym <= 'Z');
}

TokenType* IDState::GetTokenType(const std::string value){

}