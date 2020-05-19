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
    if (_currentState) {
        End();
    }

    _buffer = "";
    _buffer += symbol;

    _currentState = _transition.GetState(_currentState, symbol);
}

void StateMachine::End() {
    Token* token = _currentState->GetToken(_buffer);
    if (!token)
    {
        if (!SplitSeparators(_buffer))
            throw LexError(_buffer);
    }
    else if (token->GetType() != IGNORE)
        _tokens.emplace_back(token);
}

bool StateMachine::SplitSeparators(const std::string& buffer){
    auto separatorState = dynamic_cast<SeparatorState*>(_currentState);
    if (!separatorState)
        return false;

    std::vector<Token*> tokens = separatorState->SplitTokens(buffer);

    for (Token* token : tokens) {
        if (token == nullptr)
            return false;

        _tokens.emplace_back(token);
    }

    return true;
}

void StateMachine::PrintTokens() {
    for (Token* token : _tokens){
        token->PrintToken();
    }
}

const std::vector<Token*>& StateMachine::GetTokens() const {
    return _tokens;
}

StateMachine::~StateMachine() {
    for (auto token : _tokens)
        delete token;

    _tokens.clear();
}