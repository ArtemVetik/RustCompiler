#include "Lexer.h"

Lexer::Lexer(const std::string &fileName) : _fileName(fileName){

}

void Lexer::Analyze() {
    std::ifstream file(_fileName);

    char sym;
    while (!file.eof()){
        file.get(sym);
        if (file.eof())
            break;

        _stateMachine.TakeSymbol(sym);
    }
    _stateMachine.End();
    _stateMachine.PrintTokens();

    file.close();
}

const std::vector<Token *>& Lexer::GetTokens() const{
    return _stateMachine.GetTokens();
}