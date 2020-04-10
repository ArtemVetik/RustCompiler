#ifndef RUSTCOMPILER_IDSTATE_H
#define RUSTCOMPILER_IDSTATE_H

#include "State.h"
#include <vector>
#include <string>

class IDState : public State{
private:
    std::vector<std::string> _keyWords;
    std::vector<std::string> _idTable;

public:
    ~IDState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    void Debug() const override {std::cout << "IDState" << std::endl;}
};

#endif //RUSTCOMPILER_IDSTATE_H
