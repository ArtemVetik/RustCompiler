#include "SemanticAnalyzer.h"

void SemanticAnalyzer::Analyze() {
    Traversal(_root);
}

void SemanticAnalyzer::Traversal(Node *const &root) {
    std::vector<Node*> childs = root->GetChilds();
    for (const auto &child: childs) {
        CheckRule(child);
    }
}

void SemanticAnalyzer::CheckRule(Node* const &node) {
    if (node == nullptr)
        return;

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
        case Block:
            Traversal(node);
            break;
        case None:break;
    }
}

void SemanticAnalyzer::VariableDeclaration(Node *const &node) {

    if (node->GetChild(0)->GetData()->ruleType == RuleType::GroupVarDeclaration) {
        GroupLetVarDeclaration(node->GetChild(0));
        GroupInit(node->GetChild(1));
    }
    else {
        TypeData patType = Pat(node->GetChild(0));
        if (node->GetChild(1) != nullptr) {
            std::vector<TypeData> exprTypes = Expr(node->GetChild(1));
            if (exprTypes.size() != 1)
                throw SemanticError("Variable declaration: expected expression", node->GetChild(1)->GetData()->token.GetLocation());
            std::string id = node->GetChild(0)->GetChild(1)->GetData()->token.GetValue();
            _idTable.GetData(id).isInitialized = true;
            if (patType.type == Type::None) {
                _idTable.GetData(id).type = exprTypes[0];
                return;
            }
            if (patType != exprTypes[0])
                throw SemanticError(std::string("Type mismatch. Expected " + TypeData::ToString(patType.type)
                + ", found " + TypeData::ToString(exprTypes[0].type)), node->GetChild(1)->GetData()->token.GetLocation());
        }
    }
}

void SemanticAnalyzer::GroupLetVarDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupVarDeclaration)
        throw SemanticError("Expected group var declaration", node->GetData()->token.GetLocation());


}

void SemanticAnalyzer::GroupInit(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupInit)
        throw SemanticError("Expected group init", node->GetData()->token.GetLocation());


}

std::vector<TypeData> SemanticAnalyzer::Expr(Node *const &node) {
    std::vector<TypeData> types;

    switch (node->GetData()->ruleType) {
        case RuleType::LogicalExpression:
        case RuleType::BinaryExpression:
        case RuleType::BinaryCompExpression:
        case RuleType::UnaryExpession:
            types.emplace_back(BoolExpr(node));
            break;
        case RuleType::MemberExpression:
            types.emplace_back(MemberExpr(node));
            break;
        case RuleType::ArrayElems:
            types = ArrayElems(node);
            break;
        default:
            types.emplace_back(MinTerminal(node));
            break;
    }

    return types;
}

TypeData SemanticAnalyzer::Pat(Node *const &node) {
    ID_Data data;
    data.isMutable = node->GetChild(0) != nullptr;
    data.id = node->GetChild(1)->GetData()->token.GetValue();

    Node* type = node->GetChild(2);
    if (type == nullptr) {
        _idTable.AddToTable(data);
        return data.type;
    }

    data.type.isReference = type->GetChild(0) != nullptr;
    data.type.isMutable = type->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(type->GetChild(2)->GetData()->token.GetType());

    if (_idTable.Has(data.id))
        throw SemanticError(std::string("Variable " + data.id + " already exists"), node->GetChild(1)->GetData()->token.GetLocation());
    _idTable.AddToTable(data);

    return data.type;
}

TypeData SemanticAnalyzer::BoolExpr(Node *const &node) {

    if (node->GetData()->ruleType != LogicalExpression && node->GetData()->ruleType != BinaryExpression
        && node->GetData()->ruleType != BinaryCompExpression)
        return MinTerminal(node);

    TypeData left = BoolExpr(node->GetChild(0));
    TypeData right = BoolExpr(node->GetChild(1));
    TypeData wrongType;
    unsigned int wrongChild = 0;

    switch (node->GetData()->ruleType){
        case LogicalExpression:
            if (left.type == Type::Bool && right.type == Type::Bool)
                return TypeData(Type::Bool);
            wrongType = (left.type != Type::Bool) ? (wrongChild = 0, left) : (wrongChild = 1, right);
            throw SemanticError(std::string("Expected bool, found " + TypeData::ToString(wrongType.type)),
                    node->GetChild(wrongChild)->GetData()->token.GetLocation());
        case BinaryCompExpression:
            if (left.type == right.type)
                return TypeData(Type::Bool);
            throw SemanticError(std::string("Expected " + TypeData::ToString(left.type) + ", found " + TypeData::ToString(wrongType.type)),
                                node->GetChild(1)->GetData()->token.GetLocation());
        case BinaryExpression:
            if (left.type == right.type)
                return left;
            throw SemanticError(std::string("Expected " + TypeData::ToString(left.type) + ", found " + TypeData::ToString(right.type)),
                                node->GetChild(1)->GetData()->token.GetLocation());
        default:
            throw SemanticError("Critical error: Expected bool expression", node->GetData()->token.GetLocation());
    }


}

TypeData SemanticAnalyzer::MemberExpr(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::MemberExpression)
        throw SemanticError("Expected member expression", node->GetData()->token.GetLocation());

    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    if (!_arrayTable.Has(id))
        throw SemanticError(std::string("Variable " + id + " has not been declared"), node->GetChild(0)->GetData()->token.GetLocation());

    auto elemsTypes = ArrayElems(node->GetChild(1));
    if (elemsTypes.size() != 1)
        throw SemanticError("Expected single index in member expression", node->GetChild(1)->GetData()->token.GetLocation());
    if (elemsTypes[0].type != Type::Unsigned)
        throw SemanticError("Expected unsigned type index", node->GetChild(1)->GetChild(0)->GetData()->token.GetLocation());

    return _arrayTable.GetData(id).type;
}

std::vector<TypeData> SemanticAnalyzer::ArrayElems(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayElems)
        throw SemanticError("Expected array elements", node->GetData()->token.GetLocation());

    std::vector<TypeData> elemsTypes;
    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child: childs) {
        std::vector<TypeData> types = Expr(child);
        if (types.size() != 1)
            throw SemanticError("Array elements [] error", child->GetData()->token.GetLocation());
        elemsTypes.emplace_back(types[0]);
    }

    return elemsTypes;
}

TypeData SemanticAnalyzer::MinTerminal(Node *const &node) {
    if (node->GetData()->ruleType == RuleType::UnaryExpession)
        return MinTerminal(node->GetChild(0));

    if (node->GetData()->ruleType == RuleType::FuncInvoke)
        return FunctionInvoke(node);

    if (node->GetData()->ruleType == RuleType::InternalFuncInvoke) {
        std::string id = node->GetChild(0)->GetData()->token.GetValue();
        if (!_idTable.Has(id))
            throw SemanticError(std::string("Variable " + id + " has not been declared"),node->GetChild(0)->GetData()->token.GetLocation());
        return FunctionInvoke(node->GetChild(1));
    }

    if (node->GetData()->ruleType == RuleType::Identifier) {
        std::string id = node->GetData()->token.GetValue();
        if (!_idTable.Has(id))
            throw SemanticError(std::string("Variable " + id + " has not been declared"), node->GetData()->token.GetLocation());
        if (!_idTable.GetData(id).isInitialized)
            throw SemanticError(std::string("Variable " + id + " has not been initialized"), node->GetData()->token.GetLocation());
        return _idTable.GetData(id).type;
    }

    if (node->GetData()->ruleType == RuleType::Literal) {
        return TypeData(TypeData::ToType(node->GetData()->token.GetType()));
    }

    throw SemanticError("Critical error: Expected minimal terminal", node->GetData()->token.GetLocation());
}

TypeData SemanticAnalyzer::FunctionInvoke(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::FuncInvoke)
        throw SemanticError("Expected function invoke", node->GetData()->token.GetLocation());

    std::string functionId = node->GetChild(0)->GetData()->token.GetValue();
    if (!_functionTable.Has(functionId))
        throw SemanticError(std::string("Function " + functionId + " not found"), node->GetChild(0)->GetData()->token.GetLocation());

    std::vector<Node*> paramsNode = node->GetChild(1)->GetChilds();
    std::vector<TypeData> paramTypes;

    for (const auto &param : paramsNode) {
        if (param->GetData()->ruleType == RuleType::ArrayArg) {
            std::string arrayId = param->GetChild(2)->GetData()->token.GetValue();
            if (!_arrayTable.Has(arrayId))
                throw SemanticError(std::string("Variable " + arrayId + " has not been declared"), param->GetChild(2)->GetData()->token.GetLocation());
            paramTypes.emplace_back(_arrayTable.GetData(arrayId).type);
        }
        else {
            std::vector<TypeData> exprParam = Expr(param);
            if (exprParam.size() != 1)
                throw SemanticError("Expected single-type parameter", param->GetData()->token.GetLocation());
            paramTypes.emplace_back(exprParam[0]);
        }
    }

    std::vector<Data*> funcDefineParams = _functionTable.GetData(functionId).parameters;
    if (funcDefineParams.size() != paramTypes.size())
        throw SemanticError(std::string("Parameters count mismatch. Expected " + std::to_string(funcDefineParams.size()) + " parameters"),
                            paramsNode[paramsNode.size()-1]->GetData()->token.GetLocation());

    for (int i = 0; i < funcDefineParams.size(); ++i) {
        if (funcDefineParams[i]->type != paramTypes[i])
            throw SemanticError(std::string("Type mismatch. Expected " + TypeData::ToString(funcDefineParams[i]->type.type)),
                                paramsNode[i]->GetData()->token.GetLocation());
    }

    return _functionTable.GetData(functionId).type;
}

SemanticAnalyzer::SemanticAnalyzer(Node *const &root) {
    this->_root = root;
}
