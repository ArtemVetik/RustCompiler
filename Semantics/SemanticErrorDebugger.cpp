#include "SemanticErrorDebugger.h"

SemanticError SemanticErrorDebugger::CriticalError(const std::string &message, Node *const &root) {
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::TypeError(const std::string &expectedType, const std::string &foundType, Node *const &root) {
    std::string message = "Type mismatch. Expected " + expectedType + ", found " + foundType;
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::GroupVariableCountError(const std::string &expectedCount, Node *const &root) {
    std::string message = "Mismatch count of group elements. Expected " + expectedCount;
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::VariableExistingError(const std::string &identifier, Node *const &root) {
    std::string message = "Variable " + identifier + " already exists";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::VariableNotExistingError(const std::string &identifier, Node *const &root) {
    std::string message = "Variable " + identifier + " has not been declared";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::VariableInitializationError(const std::string &identifier, Node *const &root) {
    std::string message = "Variable " + identifier + " has not been initialized";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::VariableTypeNotExistingError(const std::string &identifier, const TokenLocation &location) {
    std::string message = "Variable " + identifier + " has not a type";
    return SemanticError(message, location);
}

SemanticError SemanticErrorDebugger::VariableImmutableError(const std::string &identifier, Node *const &root) {
    std::string message = "Can not assign twice to immutable variable " + identifier;
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::ArrayInitializationError(const std::string &identifier, Node *const &root) {
    std::string message = "Array " + identifier + " has not been initialized";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::MemberExpressionInitializationError(const std::string &identifier, Node *const &root) {
    std::string message = "Array " + identifier + " should be initialized to access member";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::ArrayCountElementsError(const std::string &expectedCount, const std::string &foundCount,
                                                             Node *const &root) {
    std::string message = "Mismatch count of array elements. Expected " + expectedCount + ", found " + foundCount;
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::FunctionExistingError(const std::string &identifier, Node *const &root) {
    std::string message = "Function " + identifier + " has already been defined in this module";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::FunctionNotExistingError(const std::string &identifier, Node *const &root) {
    std::string message = "Function " + identifier + " has not yet been defined in this module";
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::FunctionInvokeParametersCountError(const std::string &expectedCount, Node *const &root) {
    std::string message = "Parameters count mismatch. Expected " + expectedCount;
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::FunctionInvokeParametersTypeError(const std::string &expectedType, Node *const &root) {
    std::string message = "Function parameters type mismatch. Expected " + expectedType;
    return SemanticError(message, GetLocation(root));
}

SemanticError SemanticErrorDebugger::FunctionReturnTypeError(const std::string &expectedType, const std::string &foundType, Node *const &root) {
    std::string message = "Function return type mismatch. Expected " + expectedType + ", found " + foundType;
    return SemanticError(message, GetLocation(root));
}

TokenLocation* SemanticErrorDebugger::TryGetNonNullNode(Node *const &node) {
    if (node == nullptr)
        return nullptr;
    if (node->GetData()->token.GetLocation() == nullptr) {
        std::vector<Node *> childs = node->GetChilds();
        for (const auto &child : childs) {
            if (child == nullptr)
                continue;
            TokenLocation *tmp = TryGetNonNullNode(child);
            if (tmp) return tmp;
        }
    }
    return node->GetData()->token.GetLocation();
}

TokenLocation SemanticErrorDebugger::GetLocation(Node *const &node) {
    TokenLocation *location = TryGetNonNullNode(node);
    if (!location)
        return TokenLocation();
    else return *location;
}
