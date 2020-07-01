#ifndef RUSTCOMPILER_IDSTATE_H
#define RUSTCOMPILER_IDSTATE_H

#include "State.h"
#include <vector>
#include <string>

/// @file IDState.h

/// @brief Состояние идентификатора
/// @note Включает в себя любые идентификаторы, в том числе ключевые слова
class IDState : public State {
private:
    std::vector<std::string> _idTable;
    std::vector<std::pair<std::string, TokenType>> _keyWords;

    void InitKeyWords();

public:
    IDState();
    ~IDState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    [[nodiscard]] bool CanTransitTo(State* const &to) const override;
    Token* GetToken(const std::string &value) override;
};

#endif //RUSTCOMPILER_IDSTATE_H
