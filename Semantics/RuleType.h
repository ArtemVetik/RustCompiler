#ifndef RUSTCOMPILER_RULETYPE_H
#define RUSTCOMPILER_RULETYPE_H

enum RuleType {
    AssignmentExpression,
    BinaryExpression,
    LogicalExpression,
    UnaryExpession,
    MinTerminal,

    VariableDeclaration,
    GroupInit,
    GroupVarDeclaration,
    ArrayDeclaration,
    MemberExpression,
    ArrayElems,
    ArrayArg,

    FuncDeclaration,
    FuncInvoke,
    InternalFuncInvoke,

    Print,

    IfExpr,

    WhileExpr,
    LoopExpr,

    Literal,
    Identifier,

    Block,

    None,
};

#endif //RUSTCOMPILER_RULETYPE_H
