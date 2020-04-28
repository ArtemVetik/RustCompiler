#ifndef RUSTCOMPILER_STATE_H
#define RUSTCOMPILER_STATE_H

#include <iostream>
#include <algorithm>
#include "TokenType.h"
#include "Token.h"
#include <string>

class State {
public:
    virtual bool Contains(char sym) = 0;
    virtual bool CanTransitTo(char sym) = 0;
    virtual Token* GetToken(const std::string &value) = 0;
    virtual ~State() = 0;
};

#endif //RUSTCOMPILER_STATE_H
