#ifndef RUSTCOMPILER_RULETYPE_H
#define RUSTCOMPILER_RULETYPE_H

enum RuleType {
    VariableDeclaration,
    AssignmentExpression,
    BinaryExpression,
    LogicalExpression,
    UnaryExpession,

    ArrayDeclaration,
    MemberExpression,
    ArrayElemes,

    IfExpr,

    WhileExpr,
    LoopExpr,

    Literal,
    Identifier,

    Block,

    None,
};

#endif //RUSTCOMPILER_RULETYPE_H
