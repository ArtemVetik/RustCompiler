#include "StateMachine.h"

StateMachine::StateMachine() {
    _buffer = "";
    _currentState = nullptr;
}

void StateMachine::TakeSymbol(char symbol) {
    if (_currentState && _currentState->Contains(symbol)) {
        _buffer += symbol;
        return;
    }
    
    if (_currentState){
        Token* token = _currentState->GetToken(_buffer);
        if (!token)
        {
            if (!SplitSeparators(_buffer)){
                std::cout << "ERROR: " << _buffer << std::endl;
                throw -228;
            }
        }
        _tokens.emplace_back(token);
    }

    _buffer = "";
    _buffer += symbol;

    _currentState = _transition.GetState(_currentState, symbol);
}

void StateMachine::End(){
    Token* token = _currentState->GetToken(_buffer);
    if (!token)
    {
        if (!SplitSeparators(_buffer)){
            std::cout << "ERROR: " << _buffer << std::endl;
            throw -228;
        }
    }
    _tokens.emplace_back(token);
}

bool StateMachine::SplitSeparators(std::string buffer){
    SeparatorState* separatorState = dynamic_cast<SeparatorState*>(_currentState);
    if (!separatorState)
        return false;

    std::vector<Token*> tokens = separatorState->SplitTokens(buffer);

    for (int i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == nullptr)
            return false;
        _tokens.emplace_back(tokens[i]);
    }

    return true;
}