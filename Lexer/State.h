#ifndef RUSTCOMPILER_STATE_H
#define RUSTCOMPILER_STATE_H

#include <iostream>
#include <algorithm>
#include "TokenType.h"
#include "Token.h"
#include <string>

/// @file State.h

/// @brief базовый класс для всех состояний
class State {
public:
    /// @param sym входящий символ
    /// @return true - если входящий символ содержится в текущем состоянии, false - в противном случае
    virtual bool Contains(char sym) = 0;
    /// @brief проверяет возмжность перехода в текущее состояние по входящему символу
    /// @param sym входящий символ
    /// @return true - если по входящему символу можно перейти в текущее состояние, false - в противном случае
    virtual bool CanTransitTo(char sym) = 0;
    /// @brief проверяет возмжность перехода в текущее состояние из другого состояния
    /// @param to входящее состояние
    /// @return true - если по входящему состоянию можно перейти в текущее состояние, false - в противном случае
    [[nodiscard]] virtual bool CanTransitTo(State* const &to) const { return true; }
    /// @brief возвращает токен из текущего состояния
    /// @param полная лексема
    virtual Token* GetToken(const std::string &value) = 0;
    virtual ~State() = 0;
};

#endif //RUSTCOMPILER_STATE_H
