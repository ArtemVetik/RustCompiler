#ifndef RUSTCOMPILER_STATEMACHINE_H
#define RUSTCOMPILER_STATEMACHINE_H

#include "States/State.h"
#include "Transition.h"
#include "../Exceptions/LexError.h"
#include <string>
#include <cstdlib>

/// @file StateMachine.h

/// @brief Машина состояний лексического анализатора
/// @details Осуществляет контроль состояний, добавление токенов в список и смену состояний
class StateMachine {
private:
    State* _currentState;
    Transition _transition;
    std::string _buffer;
    TokenLinePosition _startPos, _endPos;
    std::vector<Token*> _tokens;

    bool SplitSeparators(const std::string& buffer);

public:
    StateMachine();
    ~StateMachine();
    /// @brief Получение символа
    /// @details Определяет по входящему символу состояние и в случае необходимости производит смену состояний
    void TakeSymbol(char symbol);
    /// @brief Формирует токен по буфферу
    void GenerateToken();
    void PrintTokens() const;

    [[nodiscard]] const std::vector<Token*>& GetTokens() const;
};

#endif //RUSTCOMPILER_STATEMACHINE_H
