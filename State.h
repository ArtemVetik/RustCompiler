#ifndef RUSTCOMPILER_STATE_H
#define RUSTCOMPILER_STATE_H

#include <iostream>

class State {
public:
    virtual bool Contains(char sym) const = 0;
    virtual bool CanTransitTo(char sym) const = 0;
    virtual void Debug() const = 0;
};

#endif //RUSTCOMPILER_STATE_H
