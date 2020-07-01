#ifndef RUSTCOMPILER_SEPARATORSTATE_H
#define RUSTCOMPILER_SEPARATORSTATE_H

#include "State.h"
#include <vector>

/// @file SeparatorState.h

/// @brief Состояние разделителей
/// @details Включает в себя комментарии
/// @note Строка разделителей: "+-*/%=><{}[];(),.:!&|"
class SeparatorState : public State {
private:
    enum class CommentType {
        SingleLine, Multiline, None
    };

    std::string _separators = "+-*/%=><{}[];(),.:!&|";
    std::vector<std::pair<std::string, TokenType>> _tokenPair;
    std::string _buffer;
    CommentType _comment;

    bool HasInTable(const std::string &value);
    void Init();

public:
    SeparatorState();
    ~SeparatorState() override = default;
    bool Contains(char sym) override;
    bool CanTransitTo(char sym) override;
    Token* GetToken(const std::string &value) override;
    /// @brief Разделяет выражение на составные разделители
    /// @code fn function()->i32{ // разделит выражение ()-> на 3 токена: '(', ')', '->'
    ///     ...
    /// } @endcode
    std::vector<Token*> SplitTokens(const std::string &value);
};

#endif //RUSTCOMPILER_SEPARATORSTATE_H
