#ifndef RUSTCOMPILER_STATEMACHINE_H
#define RUSTCOMPILER_STATEMACHINE_H

#include "State.h"
#include "Transition.h"
#include <string>
#include <cstdlib>

class StateMachine {
private:
    State* _currentState;
    Transition _transition;
    std::string _buffer;

    std::vector<Token*> _tokens;

    bool SplitSeparators(std::string buffer);
public:
    StateMachine();
    void TakeSymbol(char symbol);
    void End();
    void PrintTokens();
};


#endif //RUSTCOMPILER_STATEMACHINE_H
