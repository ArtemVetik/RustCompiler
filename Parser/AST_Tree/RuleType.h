#ifndef RUSTCOMPILER_RULETYPE_H
#define RUSTCOMPILER_RULETYPE_H

/// @file RuleType.h

/// @brief Тип правил для вершин AST дерева
enum RuleType {
    AssignmentExpression,
    BinaryExpression,
    BinaryCompExpression,
    LogicalExpression,
    UnaryExpession,

    VariableDeclaration,
    GroupInit,
    GroupVarDeclaration,
    ArrayDeclaration,
    MemberExpression,
    ArrayElems,
    ArrayArg,

    ArrayType,
    IdType,

    FuncDeclaration,
    FuncInvoke,
    InternalFuncInvoke,

    Print,

    Return,
    Break,

    IfExpr,

    WhileExpr,
    LoopExpr,

    Literal,
    Identifier,

    Block,

    None,
};

#endif //RUSTCOMPILER_RULETYPE_H
