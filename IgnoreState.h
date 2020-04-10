//
// Created by artem on 10.04.2020.
//

#ifndef RUSTCOMPILER_IGNORESTATE_H
#define RUSTCOMPILER_IGNORESTATE_H

#include "State.h"
#include <string>

class IgnoreState : public State {
private:
    std::string _ignoreSymbols = " \n\r\t";

public:
    bool Contains(char sym) const = 0;
    bool CanTransitTo(char sym) const = 0;
};


#endif //RUSTCOMPILER_IGNORESTATE_H
