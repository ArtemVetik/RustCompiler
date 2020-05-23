#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(Node *const &root) {
    this->_root = root;
}

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
        case RuleType::ArrayDeclaration:
            ArrayDeclaration(node);
            break;
        case RuleType::AssignmentExpression:
            Assignment(node);
            break;
        case Block:
            Traversal(node);
            break;
    }
}

void SemanticAnalyzer::VariableDeclaration(Node *const &node) {
    if (node->GetChild(0)->GetData()->ruleType == RuleType::GroupVarDeclaration) {
        std::vector<ID_Data> varList = GroupLetVarDeclaration(node->GetChild(0));
        if (node->GetChild(1) != nullptr)
            GroupInit(varList, node->GetChild(1));
    }
    else {
        ID_Data idData = Pat(node->GetChild(0));
        if (node->GetChild(1) != nullptr) // если есть инициализация
            VariableAssignment(idData.id, node->GetChild(1));
    }
}

std::vector<ID_Data> SemanticAnalyzer::GroupLetVarDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupVarDeclaration)
        throw SemanticError("Expected group var declaration", node->GetData()->token.GetLocation());

    std::vector<ID_Data> idList;
    std::vector<Node*> pats = node->GetChilds();
    for (const auto &pat : pats) {
        std::string id = pat->GetChild(1)->GetData()->token.GetValue();
        if (_idTable.Has(id) || _arrayTable.Has(id))
            throw SemanticError(std::string("Variable " + id + " already exists"), pat->GetChild(1)->GetData()->token.GetLocation());
        ID_Data data;
        data.isMutable = pat->GetChild(0) != nullptr;
        data.id = id;
        _idTable.AddToTable(data);
        idList.emplace_back(data);
    }

    return idList;
}

std::vector<TypeData> SemanticAnalyzer::GroupInit(std::vector<ID_Data> variables, Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupInit)
        throw SemanticError("Expected group init", node->GetData()->token.GetLocation());

    std::vector<TypeData> types;
    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child : childs) {
        types.emplace_back(Expr(child));
    }

    if (variables.size() != types.size())
        throw 2;

    for (int i = 0; i < types.size(); i++) {
        _idTable.GetData(variables[i].id).type = types[i];
        _idTable.GetData(variables[i].id).isInitialized = true;
    }

    return types;
}

TypeData SemanticAnalyzer::Expr(Node *const &node) {
    TypeData type;

    switch (node->GetData()->ruleType) {
        case RuleType::LogicalExpression:
        case RuleType::BinaryExpression:
        case RuleType::BinaryCompExpression:
        case RuleType::UnaryExpession:
            type = BoolExpr(node);
            break;
        case RuleType::MemberExpression:
            type = MemberExpr(node);
            break;
        default:
            type = MinTerminal(node);
            break;
    }

    return type;
}

ID_Data SemanticAnalyzer::Pat(Node *const &node) {
    ID_Data data;
    data.isMutable = node->GetChild(0) != nullptr;
    data.id = node->GetChild(1)->GetData()->token.GetValue();

    if (_idTable.Has(data.id) || _arrayTable.Has(data.id))
        throw SemanticError(std::string("Variable " + data.id + " already exists"), node->GetChild(1)->GetData()->token.GetLocation());

    Node* type = node->GetChild(2);
    if (type == nullptr) {
        _idTable.AddToTable(data);
        return data;
    }

    data.type.isReference = type->GetChild(0) != nullptr;
    data.type.isMutable = type->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(type->GetChild(2)->GetData()->token.GetType());
    _idTable.AddToTable(data);

    return data;
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
            throw SemanticError(std::string("Expected Bool, found " + wrongType.ToString()),
                    node->GetChild(wrongChild)->GetData()->token.GetLocation());
        case BinaryCompExpression:
            if (left.type == right.type)
                return TypeData(Type::Bool);
            throw SemanticError(std::string("Expected " + left.ToString() + ", found " + wrongType.ToString()),
                                node->GetChild(1)->GetData()->token.GetLocation());
        case BinaryExpression:
            if (left.type == right.type)
                return left;
            throw SemanticError(std::string("Expected " + left.ToString() + ", found " + right.ToString()),
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
    if (!_arrayTable.GetData(id).isInitialized)
        throw SemanticError(std::string("Array " + id + " should be initialized to access member"), node->GetChild(0)->GetData()->token.GetLocation());

    auto elemsTypes = ArrayElems(node->GetChild(1));
    if (elemsTypes.size() != 1)
        throw SemanticError("Expected single index in member expression", node->GetChild(1)->GetData()->token.GetLocation());

    std::string index = node->GetChild(1)->GetChild(0)->GetData()->token.GetValue();
    if (elemsTypes[0].type != Type::Integer || index[0] == '-')
        throw SemanticError("Expected unsigned type index", node->GetChild(1)->GetChild(0)->GetData()->token.GetLocation());

    return _arrayTable.GetData(id).type;
}

std::vector<TypeData> SemanticAnalyzer::ArrayElems(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayElems)
        throw SemanticError("Expected array elements", node->GetData()->token.GetLocation());

    std::vector<TypeData> elemsTypes;
    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child: childs) {
        TypeData type = Expr(child);
        elemsTypes.emplace_back(type);
    }

    for (unsigned int i = 1; i < elemsTypes.size(); i++) {
        if (elemsTypes[i] != elemsTypes[0])
            throw SemanticError("Different types in array elements", node->GetChild(i)->GetData()->token.GetLocation());
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

    throw SemanticError("Expected minimal terminal", node->GetData()->token.GetLocation());
}

TypeData SemanticAnalyzer::FunctionInvoke(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::FuncInvoke)
        throw SemanticError("Expected function invoke", node->GetData()->token.GetLocation());

    std::string functionId = node->GetChild(0)->GetData()->token.GetValue();
    if (!_functionTable.Has(functionId))
        throw SemanticError(std::string("Function " + functionId + " not found"), node->GetChild(0)->GetData()->token.GetLocation());

    FunctionParams(functionId, node->GetChild(1));

    return _functionTable.GetData(functionId).type;
}

void SemanticAnalyzer::ArrayDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayDeclaration)
        throw SemanticError("Expected array declaration", node->GetData()->token.GetLocation());

    Node* arrPat = node->GetChild(0);
    std::string id = arrPat->GetChild(1)->GetData()->token.GetValue();
    if (_arrayTable.Has(id) || _idTable.Has(id))
        throw SemanticError(std::string("Variable " + id + " already exists"), arrPat->GetChild(1)->GetData()->token.GetLocation());

    Array_Data data;
    data.isMutable = arrPat->GetChild(0) != nullptr;
    data.id = id;

    Node* typeNode = arrPat->GetChild(2)->GetChild(0);
    data.type.isReference = typeNode->GetChild(0) != nullptr;
    data.type.isMutable = typeNode->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(typeNode->GetChild(2)->GetData()->token.GetType());
    data.elementCount = std::stoul(arrPat->GetChild(2)->GetChild(1)->GetData()->token.GetValue());
    _arrayTable.AddToTable(data);

    if (node->GetChild(1) != nullptr)
        ArrayAssignment(data.id, node->GetChild(1));
}

void SemanticAnalyzer::Assignment(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::AssignmentExpression)
        throw SemanticError("Expected assignment expression", node->GetData()->token.GetLocation());

    if (node->GetChild(0)->GetData()->ruleType == RuleType::MemberExpression) {
        MemberExpr(node->GetChild(0));
        std::string arrId = node->GetChild(0)->GetChild(0)->GetData()->token.GetValue();
        MemberAssignment(arrId, node->GetChild(1));
    }
    else if (node->GetChild(0)->GetData()->ruleType == RuleType::Identifier) {
        std::string id = node->GetChild(0)->GetData()->token.GetValue();
        if (_idTable.Has(id))
            VariableAssignment(id, node->GetChild(1));
        else if (_arrayTable.Has(id))
            ArrayAssignment(id, node->GetChild(1));
        else
            throw SemanticError(std::string("Variable " + id + " has not been declared"), node->GetChild(0)->GetData()->token.GetLocation());
    }
    else
        throw SemanticError("Critical error: can not assigment", node->GetData()->token.GetLocation());
}

void SemanticAnalyzer::VariableAssignment(std::string id, Node *const &exprNode) {
    if (!_idTable.Has(id))
        throw 2;
    ID_Data idData = _idTable.GetData(id);

    if (!idData.isMutable && idData.isInitialized)
        throw 2;
    if (exprNode->GetData()->ruleType == RuleType::ArrayElems)
        throw 2;

    TypeData exprType = Expr(exprNode);

    if (idData.type.type == Type::None)
        _idTable.GetData(id).type = exprType;
    else if (idData.type != exprType)
        throw 2;

    _idTable.GetData(id).isInitialized = true;
}

void SemanticAnalyzer::ArrayAssignment(std::string id, Node *const &exprNode) {
    if (!_arrayTable.Has(id))
        throw 2;
    Array_Data arrData = _arrayTable.GetData(id);

    if (!arrData.isMutable && arrData.isInitialized)
        throw 2;

    std::vector<TypeData> exprTypes = ArrayElems(exprNode);
    if (exprTypes.size() != arrData.elementCount)
        throw 2;
    if (arrData.type != exprTypes[0])
        throw 2;

    _arrayTable.GetData(id).isInitialized = true;
}

void SemanticAnalyzer::MemberAssignment(std::string id, Node *const &exprNode) {
    if (!_arrayTable.Has(id))
        throw 2;
    Array_Data arrData = _arrayTable.GetData(id);

    if (!arrData.isMutable && arrData.isInitialized)
        throw 2;

    TypeData exprType = Expr(exprNode);
    if (arrData.type != exprType)
        throw 2;
}

std::vector<TypeData> SemanticAnalyzer::FunctionParams(std::string funcId, Node *const &node) {
    std::vector<Node*> paramsNode = node->GetChilds();
    std::vector<TypeData> paramTypes;

    for (const auto &param : paramsNode) {
        if (param->GetData()->ruleType == RuleType::ArrayArg)
            paramTypes.emplace_back(CanAccessArray(param->GetChild(2)));
        else if (param->GetData()->ruleType == RuleType::Identifier)
            paramTypes.emplace_back(CanAccessIdentifier(param));
        else
            paramTypes.emplace_back(Expr(param));
    }

    std::vector<Data*> funcDefineParams = _functionTable.GetData(funcId).parameters;
    if (funcDefineParams.size() != paramTypes.size())
        throw SemanticError(std::string("Parameters count mismatch. Expected " + std::to_string(funcDefineParams.size()) + " parameters"),
                            paramsNode[paramsNode.size()-1]->GetData()->token.GetLocation());

    for (int i = 0; i < funcDefineParams.size(); ++i) {
        if (funcDefineParams[i]->type != paramTypes[i])
            throw SemanticError(std::string("Type mismatch. Expected " + funcDefineParams[i]->type.ToString()),
                                paramsNode[i]->GetData()->token.GetLocation());
    }

    return paramTypes;
}

TypeData SemanticAnalyzer::CanAccessIdentifier(Node *const &idNode) {
    std::string id = idNode->GetData()->token.GetValue();
    if (!_idTable.Has(id))
        throw 2;
    if (!_idTable.GetData(id).isInitialized)
        throw 2;

    return _idTable.GetData(id).type;
}

TypeData SemanticAnalyzer::CanAccessArray(Node *const &idNode) {
    std::string id = idNode->GetData()->token.GetValue();
    if (!_arrayTable.Has(id))
        throw 2;
    if (!_arrayTable.GetData(id).isInitialized)
        throw 2;

    return _arrayTable.GetData(id).type;
}