#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(Node *const &root) {
    this->_root = root;
    _currentBlock = new ProgramBlock();
}

SemanticAnalyzer::~SemanticAnalyzer() {
    if (_currentBlock) {
        delete _currentBlock;
        _currentBlock = nullptr;
    }
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
        case RuleType::Block:
            _currentBlock = &_currentBlock->AddBlock();
            Traversal(node);
            _currentBlock = _currentBlock->upperBlock;
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
            VariableAssignment(node->GetChild(0)->GetChild(1), node->GetChild(1));
    }
}

std::vector<ID_Data> SemanticAnalyzer::GroupLetVarDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupVarDeclaration)
        throw Err::CriticalError("Expected group var declaration", node);

    std::vector<ID_Data> idList;
    std::vector<Node*> pats = node->GetChilds();
    for (const auto &pat : pats) {
        std::string id = pat->GetChild(1)->GetData()->token.GetValue();
        if (_currentBlock->idTable.Has(id) || _currentBlock->arrayTable.Has(id))
            throw Err::VariableExistingError(id, pat->GetChild(1));
        ID_Data data;
        data.isMutable = pat->GetChild(0) != nullptr;
        data.id = id;
        _currentBlock->idTable.AddToTable(data);
        idList.emplace_back(data);
    }

    return idList;
}

std::vector<TypeData> SemanticAnalyzer::GroupInit(std::vector<ID_Data> variables, Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupInit)
        throw Err::CriticalError("Expected group initialize", node);

    std::vector<TypeData> types;
    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child : childs) {
        types.emplace_back(Expr(child));
    }

    if (variables.size() != types.size())
        throw Err::GroupVariableCountError(std::to_string(variables.size()), node);

    for (int i = 0; i < types.size(); i++) {
        _currentBlock->idTable.GetData(variables[i].id).type = types[i];
        _currentBlock->idTable.GetData(variables[i].id).isInitialized = true;
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

    if (_currentBlock->idTable.Has(data.id) || _currentBlock->arrayTable.Has(data.id))
        throw Err::VariableExistingError(data.id, node->GetChild(1));

    Node* type = node->GetChild(2);
    if (type == nullptr) {
        _currentBlock->idTable.AddToTable(data);
        return data;
    }

    data.type.isReference = type->GetChild(0) != nullptr;
    data.type.isMutable = type->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(type->GetChild(2)->GetData()->token.GetType());
    _currentBlock->idTable.AddToTable(data);

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
            throw Err::TypeError("Bool", wrongType.ToString(), node->GetChild(wrongChild));
        case BinaryCompExpression:
            if (left.type == right.type)
                return TypeData(Type::Bool);
            throw Err::TypeError(left.ToString(), right.ToString(), node->GetChild(1));
        case BinaryExpression:
            if (left.type == right.type)
                return left;
            throw Err::TypeError(left.ToString(), right.ToString(), node->GetChild(1));
        default:
            throw Err::CriticalError("Expected bool expression", node);
    }
}

TypeData SemanticAnalyzer::MemberExpr(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::MemberExpression)
        throw Err::CriticalError("Expected member expression", node);

    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    if (!HasArrInUpper(id, node->GetChild(0)))
        throw Err::VariableNotExistingError(id, node->GetChild(0));
    if (!GetArr(id, node->GetChild(0)).isInitialized)
        throw Err::MemberExpressionInitializationError(id, node->GetChild(0));

    auto elemsTypes = ArrayElems(node->GetChild(1));
    if (elemsTypes.size() != 1)
        throw Err::CriticalError("Expected single index in member expression", node->GetChild(1));

    std::string index = node->GetChild(1)->GetChild(0)->GetData()->token.GetValue();
    if (elemsTypes[0].type != Type::Integer || index[0] == '-')
        throw Err::CriticalError("Expected unsigned type index", node->GetChild(1)->GetChild(0));

    return GetArr(id,node->GetChild(0)).type;
}

std::vector<TypeData> SemanticAnalyzer::ArrayElems(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayElems)
        throw Err::CriticalError("Expected array elements", node);

    std::vector<TypeData> elemsTypes;
    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child: childs) {
        TypeData type = Expr(child);
        elemsTypes.emplace_back(type);
    }

    for (unsigned int i = 1; i < elemsTypes.size(); i++) {
        if (elemsTypes[i] != elemsTypes[0])
            throw Err::CriticalError("Different types in array elements", node->GetChild(i));
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
        if (!HasIDInUpper(id, node->GetChild(0)))
            throw Err::VariableNotExistingError(id, node->GetChild(0));
        return FunctionInvoke(node->GetChild(1));
    }

    if (node->GetData()->ruleType == RuleType::Identifier) {
        std::string id = node->GetData()->token.GetValue();
        if (HasArrInUpper(id, node))
            throw Err::CriticalError("It is impossible to assign an array to an ordinary variable", node);
        if (!GetID(id, node).isInitialized)
            throw Err::VariableInitializationError(id, node);
        return GetID(id, node).type;
    }

    if (node->GetData()->ruleType == RuleType::Literal) {
        return TypeData(TypeData::ToType(node->GetData()->token.GetType()));
    }

    throw Err::CriticalError("Expected minimal terminal", node);
}

TypeData SemanticAnalyzer::FunctionInvoke(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::FuncInvoke)
        throw Err::CriticalError("Expected function invoke", node);

    std::string functionId = node->GetChild(0)->GetData()->token.GetValue();
    if (!_functionTable.Has(functionId))
        throw Err::FunctionNotExistingError(functionId, node);

    FunctionParams(functionId, node->GetChild(1));

    return _functionTable.GetData(functionId).type;
}

void SemanticAnalyzer::ArrayDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayDeclaration)
        throw Err::CriticalError("Expected array declaration", node);

    Node* arrPat = node->GetChild(0);
    std::string id = arrPat->GetChild(1)->GetData()->token.GetValue();
    if (_currentBlock->arrayTable.Has(id) || _currentBlock->idTable.Has(id))
        throw Err::VariableExistingError(id, arrPat->GetChild(1));

    Array_Data data;
    data.isMutable = arrPat->GetChild(0) != nullptr;
    data.id = id;

    Node* typeNode = arrPat->GetChild(2)->GetChild(0);
    data.type.isReference = typeNode->GetChild(0) != nullptr;
    data.type.isMutable = typeNode->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(typeNode->GetChild(2)->GetData()->token.GetType());
    data.elementCount = std::stoul(arrPat->GetChild(2)->GetChild(1)->GetData()->token.GetValue());
    _currentBlock->arrayTable.AddToTable(data);

    if (node->GetChild(1) != nullptr)
        ArrayAssignment(arrPat->GetChild(1), node->GetChild(1));
}

void SemanticAnalyzer::Assignment(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::AssignmentExpression)
        throw Err::CriticalError("Expected assignment expression", node);

    if (node->GetChild(0)->GetData()->ruleType == RuleType::MemberExpression) {
        MemberExpr(node->GetChild(0));
        Node* arrIdNode = node->GetChild(0)->GetChild(0);
        MemberAssignment(arrIdNode, node->GetChild(1));
    }
    else if (node->GetChild(0)->GetData()->ruleType == RuleType::Identifier) {
        std::string id = node->GetChild(0)->GetData()->token.GetValue();
        if (HasIDInUpper(id, node->GetChild(0)))
            VariableAssignment(node->GetChild(0), node->GetChild(1));
        else if (HasArrInUpper(id, node->GetChild(0)))
            ArrayAssignment(node->GetChild(0), node->GetChild(1));
        else
            throw Err::VariableNotExistingError(id, node->GetChild(0));
    }
    else
        throw Err::CriticalError("Critical _error: can not assigment", node);
}

void SemanticAnalyzer::VariableAssignment(Node* const& idNode, Node *const &exprNode) {
    std::string id = idNode->GetData()->token.GetValue();

    if (!HasIDInUpper(id, idNode))
        throw Err::VariableNotExistingError(id, idNode);

    ID_Data &idData = GetID(id, idNode);
    if (!idData.isMutable && idData.isInitialized)
        throw Err::VariableImmutableError(id, idNode);

    TypeData exprType = Expr(exprNode);

    if (idData.type.type == Type::None)
        idData.type = exprType;
    else if (idData.type != exprType)
        throw Err::TypeError(idData.type.ToString(), exprType.ToString(), exprNode);

    idData.isInitialized = true;
}

void SemanticAnalyzer::ArrayAssignment(Node* const& idNode, Node *const &exprNode) {
    std::string id = idNode->GetData()->token.GetValue();
    if (!HasArrInUpper(id, idNode))
        throw Err::VariableNotExistingError(id, idNode);
    Array_Data &arrData = GetArr(id, idNode);

    if (!arrData.isMutable && arrData.isInitialized)
        throw Err::VariableImmutableError(id, idNode);

    std::vector<TypeData> exprTypes = ArrayElems(exprNode);
    if (exprTypes.size() != arrData.elementCount)
        throw Err::ArrayCountElementsError(std::to_string(arrData.elementCount), std::to_string(exprTypes.size()), exprNode->GetChild(exprTypes.size()-1));
    if (arrData.type != exprTypes[0])
        throw Err::TypeError(arrData.type.ToString(), exprTypes[0].ToString(), exprNode);

    arrData.isInitialized = true;
}

void SemanticAnalyzer::MemberAssignment(Node* const& idNode, Node *const &exprNode) {
    std::string id = idNode->GetData()->token.GetValue();
    if (!HasArrInUpper(id, idNode))
        throw Err::VariableNotExistingError(id, idNode);
    Array_Data &arrData = GetArr(id, idNode);

    if (!arrData.isMutable && arrData.isInitialized)
        throw Err::VariableImmutableError(id, idNode);

    TypeData exprType = Expr(exprNode);
    if (arrData.type != exprType)
        throw Err::TypeError(arrData.type.ToString(), exprType.ToString(), exprNode);
}

std::vector<std::pair<TypeData, bool>> SemanticAnalyzer::FunctionParams(const std::string &funcId, Node *const &node) {
    std::vector<Node*> paramsNode = node->GetChilds();
    std::vector<std::pair<TypeData, bool>> paramTypes;

    for (const auto &param : paramsNode) {
        if (param->GetData()->ruleType == RuleType::ArrayArg)
            paramTypes.emplace_back(std::make_pair(CanAccessArray(param->GetChild(2)), true));
        else if (param->GetData()->ruleType == RuleType::Identifier)
            paramTypes.emplace_back(std::make_pair(CanAccessIdentifier(param), false));
        else
            paramTypes.emplace_back(std::make_pair(Expr(param), false));
    }

    std::vector<Data*> funcDefineParams = _functionTable.GetData(funcId).parameters;
    if (funcDefineParams.size() != paramTypes.size())
        throw Err::FunctionInvokeParametersCountError(std::to_string(funcDefineParams.size()), paramsNode[paramsNode.size()-1]);

    for (unsigned int i = 0; i < funcDefineParams.size(); ++i) {
        if (funcDefineParams[i]->type != paramTypes[i].first)
            throw Err::FunctionInvokeParametersTypeError(funcDefineParams[i]->type.ToString(), paramsNode[i]);
        if (paramTypes[i].second && reinterpret_cast<Array_Data*>(funcDefineParams[i]) == nullptr)
            throw Err::CriticalError("It is impossible to pass an array instead of a regular variable", node->GetChild(i));
        if (!paramTypes[i].second && reinterpret_cast<ID_Data*>(funcDefineParams[i]) == nullptr)
            throw Err::CriticalError("", node->GetChild(i));
    }

    return paramTypes;
}

TypeData SemanticAnalyzer::CanAccessIdentifier(Node *const &idNode) {
    std::string id = idNode->GetData()->token.GetValue();
    if (!HasIDInUpper(id, idNode))
        throw Err::VariableNotExistingError(id, idNode);

    ID_Data &idData = GetID(id, idNode);
    if (!idData.isInitialized)
        throw Err::VariableInitializationError(id, idNode);

    return idData.type;
}

TypeData SemanticAnalyzer::CanAccessArray(Node *const &idNode) {
    std::string id = idNode->GetData()->token.GetValue();
    if (!HasArrInUpper(id, idNode))
        throw Err::VariableNotExistingError(id, idNode);

    Array_Data &arrayData = GetArr(id, idNode);
    if (!arrayData.isInitialized)
        throw Err::ArrayInitializationError(id, idNode);

    return arrayData.type;
}

bool SemanticAnalyzer::HasIDInUpper(const std::string &id, Node *const &root) {
    ProgramBlock *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return true;
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    return false;
}

bool SemanticAnalyzer::HasArrInUpper(const std::string &id, Node *const &root) {
    ProgramBlock *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return true;
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    return false;
}

ID_Data& SemanticAnalyzer::GetID(const std::string &id, Node* const &root) {
    ProgramBlock *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return tmp->idTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    throw Err::VariableNotExistingError(id, root);
}

Array_Data& SemanticAnalyzer::GetArr(const std::string &id, Node* const &root) {
    ProgramBlock *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return tmp->arrayTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    throw Err::VariableNotExistingError(id, root);
}