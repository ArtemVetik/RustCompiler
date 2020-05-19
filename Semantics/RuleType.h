#ifndef RUSTCOMPILER_RULETYPE_H
#define RUSTCOMPILER_RULETYPE_H

enum RuleType {
    AssignmentExpression,
    BinaryExpression,
    LogicalExpression,
    UnaryExpession,


    VariableDeclaration,
    GroupInit,
    GroupVarDeclaration,
    ArrayDeclaration,
    MemberExpression,
    ArrayElems,

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
