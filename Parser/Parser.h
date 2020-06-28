#ifndef RUSTCOMPILER_PARSER_H
#define RUSTCOMPILER_PARSER_H

#include <string>
#include <vector>
#include "../Exceptions/ParserError.h"
#include "../Lexer/Token.h"
#include "AST_Tree/AST_Tree.h"
#include "AST_Tree/Node.h"

/// @file Parser.h

/// @brief класс синтаксического анализатора
/// @details осуществляе разбор токенов, проверку на синтаксическую корректность и построение абстактного синтаксического дерева
class Parser {
private:
    std::vector<Token*> _tokens;
    std::vector<Token*>::iterator _currentToken;
    AST_Tree _tree;

    // Объявление переменных
    bool LetDecl(Node *&root);
    bool Pat(Node *&root);
    bool GroupLet(Node *&root);
    bool VarList(Node *&root);
    bool Init(Node *&root);
    bool Expr(Node *&root);
    bool SimpleExpr(Node *&root);
    bool MemberExpr(Node *&root);
    bool GroupInit(Node *&root);
    bool LitList(Node *&root);
    bool Type(Node *&root);

    // Ветвления
    bool IfExpr(Node *&root);
    bool ElseTail(Node *&root);
    bool BoolExpr(Node *&root);
    bool BoolExprOr(Node *&root);
    bool BoolExprComp(Node *&root);
    bool Add(Node *&root);
    bool Mult(Node *&root);
    bool MinTerminal(Node *&root);
    bool IsLiteral(Node *&root);

    // Терминалы
    bool IsString(Node *&root);
    bool IsNum(Node *&root);
    bool IsBool(Node *&root);
    bool IsID(Node *&root);

    // Знаки отношения
    bool IsCompOperation(Node *&root);

    // Вывод
    bool Println(Node *&root);
    bool ExprList(Node *&root);

    // Массивы
    bool LetArrayDecl(Node *&root);
    bool ArrayType(Node *&root);
    bool ArrayExpr(Node *&root);
    bool ArrayElems(Node *&root);

    // Циклы
    bool WhileExpr(Node *&root);
    bool LoopExpr(Node *&root);

    // Вызов функций
    bool FunctionInvoke(Node *&root);
    bool FuncArgument(Node *&root);
    bool ArrayFuncArgument(Node *&root);
    bool InternalFunctionInvoke(Node *&root);

    // Определение функций
    bool FunctionDefine(Node *&root);
    bool FunctionDefineArg(Node *&root);
    bool FunctionReturn(Node *&root);
    bool FunctionParameters(Node *&pNode);

    // Присваивание
    bool Assignment(Node *&root);

    // Выход из блока
    bool BlockExit(Node *&root);

    // Блок - внутри всё что угодно
    bool Block(Node *&root);
    bool NestedBlock(Node *& root);
    bool BlockChecker(Node *&root);

    bool TryGetToken(const TokenType &type);
public:
    /// @param tokens список токенов по которым будет осуществляться разбор
    explicit Parser(const std::vector<Token *> &tokens);

    /// @brief запускает синтаксический анализ
    bool Analyze();
    /// @return абстрактное синтаксическое дерево
    [[nodiscard]] const AST_Tree &GetASTTree() const;
};

#endif //RUSTCOMPILER_PARSER_H
