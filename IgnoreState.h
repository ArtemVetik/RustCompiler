#ifndef RUSTCOMPILER_IGNORESTATE_H
#define RUSTCOMPILER_IGNORESTATE_H

#include "State.h"
#include <string>

class IgnoreState : public State {
private:
    std::string _ignoreSymbols = " \n\r\t";

public:
    ~IgnoreState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    void Debug() const override {std::cout << "IgnoreState" << std::endl;}
};

#endif //RUSTCOMPILER_IGNORESTATE_H