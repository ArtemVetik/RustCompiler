#include "SemanticAnalyzer.h"

void SemanticAnalyzer::Analyze(Node* const &root) {
    Traversal(root);
}

void SemanticAnalyzer::Traversal(Node *const &root) {
    CheckRule(root);

    std::vector<Node*> childs = root->GetChilds();
    for (const auto &node : childs) {
        Traversal(node);
    }
}

void SemanticAnalyzer::CheckRule(Node* const &node) {
    switch (node->GetData()->ruleType) {
        case RuleType::VariableDeclaration:
            VariableDeclaration(node);
            break;
        case AssignmentExpression:break;
        case BinaryExpression:break;
        case LogicalExpression:break;
        case UnaryExpession:break;
        case ArrayDeclaration:break;
        case MemberExpression:break;
        case RuleType::ArrayElems: break;
        case FuncDeclaration:break;
        case FuncInvoke:break;
        case InternalFuncInvoke:break;
        case Print:break;
        case IfExpr:break;
        case WhileExpr:break;
        case LoopExpr:break;
        case Literal:break;
        case Identifier:break;
        case Block:break;
        case None:break;
    }
}

void SemanticAnalyzer::VariableDeclaration(Node *const &node) {

    if (node->GetChild(0)->GetData()->ruleType == RuleType::GroupVarDeclaration) {
        GroupLetVarDeclaration(node->GetChild(0));
        GroupInit(node->GetChild(1));
    }
    else {
        Type patType = Pat(node->GetChild(0));
        if (node->GetChild(1) != nullptr) {
            Expr(node->GetChild(1));

            // TODO: check type
        }
    }
}

void SemanticAnalyzer::GroupLetVarDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupVarDeclaration)
        throw SemanticError("Expected group var declaration");
}

void SemanticAnalyzer::GroupInit(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupInit)
        throw SemanticError("Expected group init");
}

const std::vector<Type>& SemanticAnalyzer::Expr(Node *const &node) {
    std::vector<Type> types;

    switch (node->GetData()->ruleType) {
        case RuleType::LogicalExpression:
        case RuleType::BinaryExpression:
        case RuleType::UnaryExpession:
            types.emplace_back(BoolExpr(node));
            break;
        case RuleType::MemberExpression:
            types.emplace_back(MemberExpr(node));
            break;
        case RuleType::ArrayElems:
            types = ArrayExpr(node);
            break;
        default:
            throw SemanticError("Expected expression");
    }

    return types;
}

Type SemanticAnalyzer::Pat(Node *const &node) {
    ID_Data data;
    data.isMutable = node->GetChild(0) != nullptr;
    data.id = node->GetChild(1)->GetData()->token.GetValue();

    Node* type = node->GetChild(2);
    if (type == nullptr){
        return Type::None;
    }

    data.type.isReference = type->GetChild(0) != nullptr;
    data.type.isMutable = type->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(type->GetChild(2)->GetData()->token.GetType());

    _idTable.AddToTable(data);

    return data.type.type;
}

Type SemanticAnalyzer::BoolExpr(Node *const &node) {

}

const std::vector<Type>& SemanticAnalyzer::ArrayExpr(Node *const &node) {

}

Type SemanticAnalyzer::MemberExpr(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::MemberExpression)
        throw SemanticError("Expected member expression");

    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    if (!_arrayTable.Has(id))
        throw SemanticError(std::string("Variable " + id + " not found"));

    ArrayElems(node->GetChild(1));

    return _arrayTable.GetData(id).type.type;
}

void SemanticAnalyzer::ArrayElems(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayElems)
        throw SemanticError("Expected array elements");

    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child: childs) {
        std::vector<Type> types = Expr(child);
        if (types.size() != 1)
            throw SemanticError("Array elements [] error");
        if (types[0] != Type::Unsigned)
            throw SemanticError("Expected unsigned type");
    }
}
