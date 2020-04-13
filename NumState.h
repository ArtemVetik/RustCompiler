#ifndef RUSTCOMPILER_NUMSTATE_H
#define RUSTCOMPILER_NUMSTATE_H

#include "State.h"

class NumState : public State {
public:
    ~NumState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    Token* GetToken(std::string value) override;
};

#endif //RUSTCOMPILER_NUMSTATE_H
