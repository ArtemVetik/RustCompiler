#ifndef RUSTCOMPILER_LEXER_H
#define RUSTCOMPILER_LEXER_H


#include "StateMachine.h"
#include <string>
#include <fstream>

class Lexer {
private:
     StateMachine _stateMachine;
     std::string _fileName;

public:
    explicit Lexer(std::string fileName);
    void Analyze();
    const std::vector<Token*>& GetTokens() const;
};


#endif //RUSTCOMPILER_LEXER_H
