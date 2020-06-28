#ifndef RUSTCOMPILER_TRANSITION_H
#define RUSTCOMPILER_TRANSITION_H

#include "State.h"
#include "IDState.h"
#include "NumState.h"
#include "IgnoreState.h"
#include "StringState.h"
#include "SeparatorState.h"
#include "../Exceptions/LexError.h"
#include <string>
#include <vector>

/// @file Transition.h

/// @brief класс, осузествляющий переходы между состояниями
class Transition {
private:
    std::vector<State*> _states;

public:
    Transition();
    ~Transition();

    /// @param currentState текущее состояние
    /// @param symbol следущий символ
    State* GetState(State* const &currentState, char symbol);
};

#endif //RUSTCOMPILER_TRANSITION_H
