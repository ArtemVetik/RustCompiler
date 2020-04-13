#ifndef RUSTCOMPILER_IDSTATE_H
#define RUSTCOMPILER_IDSTATE_H

#include "State.h"
#include <vector>
#include <string>

class IDState : public State{
private:
    std::vector<std::string> _idTable;
    std::vector<std::pair<std::string, TokenType>> _keyWords;

public:
    IDState();
    ~IDState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    Token* GetToken(const std::string &value) override;
    void InitKeyWords();
};

#endif //RUSTCOMPILER_IDSTATE_H
