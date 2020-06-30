#ifndef RUSTCOMPILER_TRANSITION_H
#define RUSTCOMPILER_TRANSITION_H

#include "States/State.h"
#include "States/IDState.h"
#include "States/NumState.h"
#include "States/IgnoreState.h"
#include "States/StringState.h"
#include "States/SeparatorState.h"
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
