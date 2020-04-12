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
            if (dynamic_cast<SeparatorState*>(_currentState)){
                for (int i = 0; i < _buffer.size(); ++i) {
                    token = _currentState->GetToken(std::string(1,_buffer[i]));
                    if (!token){
                        std::cout << "ERROR1: " << _buffer[i] << std::endl;
                        throw  -227;
                    }
                    std::cout << token->AAAPrintTokenType(token->Type()) << "\t" << token->Value() << std::endl;
                    std::cout << std::string(20,'-') << std::endl;
                }
            } else{
                std::cout << "ERROR: " << _buffer << std::endl;
                throw -228;
            }
        }
        else if (token->Type() != IGNORE){
            std::cout << token->AAAPrintTokenType(token->Type()) << "\t" << token->Value() << std::endl;
            std::cout << std::string(20,'-') << std::endl;
        }
    }

    _buffer = "";
    _buffer += symbol;

    _currentState = _transition.GetState(_currentState, symbol);
}

void StateMachine::End(){
    Token* token = _currentState->GetToken(_buffer);
    if (!token){
        std::cout << "ERROR: " << _buffer << std::endl;
        throw -228;
    }
    if (token->Type() != IGNORE){
        std::cout << token->AAAPrintTokenType(token->Type()) << "\t" << token->Value() << std::endl;
        std::cout << std::string(20,'-') << std::endl;
    }
}