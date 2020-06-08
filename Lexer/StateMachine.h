#ifndef RUSTCOMPILER_STATEMACHINE_H
#define RUSTCOMPILER_STATEMACHINE_H

#include "State.h"
#include "Transition.h"
#include "../Exceptions/LexError.h"
#include <string>
#include <cstdlib>

class StateMachine {
private:
    State* _currentState;
    Transition _transition;
    std::string _buffer;
    TokenLinePosition _startPos, _endPos;
    std::vector<Token*> _tokens;

    bool SplitSeparators(const std::string& buffer);
public:
    StateMachine();
    ~StateMachine();
    void TakeSymbol(char symbol);
    void End();
    void PrintTokens() const;

    const std::vector<Token*>& GetTokens() const;
};


#endif //RUSTCOMPILER_STATEMACHINE_H
