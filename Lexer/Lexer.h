#ifndef RUSTCOMPILER_LEXER_H
#define RUSTCOMPILER_LEXER_H

#include "StateMachine.h"
#include <string>
#include <fstream>
#include <utility>

/// @file Lexer.h

/// @brief Основной класс лексического анализатора
class Lexer {
private:
     StateMachine _stateMachine;
     std::string _fileName;

public:
    explicit Lexer(std::string fileName);
    /// @brief Запускает лексический анализ
    void Analyze();
    /// @return список всех лексем
    [[nodiscard]] const std::vector<Token*>& GetTokens() const;
};

#endif //RUSTCOMPILER_LEXER_H
