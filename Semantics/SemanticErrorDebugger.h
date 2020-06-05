#ifndef RUSTCOMPILER_SEMANTICERRORDEBUGGER_H
#define RUSTCOMPILER_SEMANTICERRORDEBUGGER_H

#include <string>
#include <iostream>
#include <vector>
#include "../AST_Tree/AST_Tree.h"
#include "../Exceptions/SemanticError.h"
#include "../Exceptions/TokenLinePosition.h"

class SemanticErrorDebugger final {
private:
    static TokenLocation* TryGetNonNullNode(Node *const &node);
    static TokenLocation GetLocation(Node *const &node);

public:
    SemanticErrorDebugger() = delete;
    static SemanticError CriticalError(const std::string &message, Node *const &root);
    static SemanticError TypeError(const std::string &expectedType, const std::string &foundType, Node *const &root);
    static SemanticError GroupVariableCountError(const std::string &identifier, Node *const &root);
    static SemanticError VariableExistingError(const std::string &identifier, Node *const &root);
    static SemanticError VariableNotExistingError(const std::string &identifier, Node *const &root);
    static SemanticError VariableInitializationError(const std::string &identifier, Node *const &root);
    static SemanticError VariableTypeNotExistingError(const std::string &identifier, const TokenLocation &location);
    static SemanticError VariableImmutableError(const std::string &identifier, Node *const &root);
    static SemanticError ArrayInitializationError(const std::string &identifier, Node *const &root);
    static SemanticError ArrayCountElementsError(const std::string &expectedCount, const std::string &foundCount, Node *const &root);
    static SemanticError MemberExpressionInitializationError(const std::string &identifier, Node *const &root);
    static SemanticError FunctionExistingError(const std::string &identifier, Node *const &root);
    static SemanticError FunctionNotExistingError(const std::string &identifier, Node *const &root);
    static SemanticError FunctionInvokeParametersCountError(const std::string &expectedCount, Node *const &root);
    static SemanticError FunctionInvokeParametersTypeError(const std::string &expectedType, Node *const &root);
    static SemanticError FunctionReturnTypeError(const std::string &expectedType, const std::string &foundType, Node *const &root);
};

#endif //RUSTCOMPILER_SEMANTICERRORDEBUGGER_H
