#ifndef RUSTCOMPILER_STRINGSTATE_H
#define RUSTCOMPILER_STRINGSTATE_H

#include "State.h"
#include <string>

class StringState : public State{
private:
    bool _isActive;

public:
    StringState();

    ~StringState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    void Debug() const override {std::cout << "StringState" << std::endl;}


};

#endif //RUSTCOMPILER_STRINGSTATE_H
