#ifndef RUSTCOMPILER_RULETYPE_H
#define RUSTCOMPILER_RULETYPE_H

enum RuleType {
    AssignmentExpression,
    BinaryExpression,
    BinaryCompExpression,
    LogicalExpression,
    UnaryExpession,

    Num,

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
