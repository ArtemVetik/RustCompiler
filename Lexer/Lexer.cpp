#include "Lexer.h"

#include <utility>

Lexer::Lexer(std::string fileName) : _fileName(std::move(fileName)){

}

void Lexer::Analyze() {
    std::ifstream file(_fileName);

    file.seekg(0, std::ios_base::end);
    long file_size = file.tellg();
    if (file_size == 0)
        throw LexError("File is empty", TokenLocation());
    file.seekg(0, std::ios_base::beg);

    char sym;
    while (!file.eof()) {
        file.get(sym);
        if (file.eof())
            break;

        _stateMachine.TakeSymbol(sym);
    }
    _stateMachine.End();
    _stateMachine.PrintTokens();

    file.close();
}

const std::vector<Token*>& Lexer::GetTokens() const {
    return _stateMachine.GetTokens();
}
