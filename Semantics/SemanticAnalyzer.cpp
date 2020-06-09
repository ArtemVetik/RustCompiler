#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(const AST_Tree &tree) {
    this->_root = tree.GetRoot();
    _currentBlock = new ProgramBlock<ID_Data, Array_Data>();
    AddSystemFunctions();
}

SemanticAnalyzer::~SemanticAnalyzer() {
    while (_currentBlock->upperBlock)
       _currentBlock = _currentBlock->upperBlock;

    if (_currentBlock) {
        delete _currentBlock;
        _currentBlock = nullptr;
    }
}

void SemanticAnalyzer::Analyze() {
    Traversal(_root);

    auto *tmp = _currentBlock;
    while (tmp->upperBlock)
        tmp = tmp->upperBlock;

    CheckInits(*tmp);
}

void SemanticAnalyzer::Traversal(Node *const &root) {
    if (root == nullptr)
        return;
    const std::vector<Node*> &childs = root->GetChilds();

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
        case RuleType::LoopExpr:
            CheckRule(node->GetChild(0));
            break;
        case RuleType::WhileExpr:
            Condition(node->GetChild(0));
            Traversal(node->GetChild(1));
            break;
        case RuleType::IfExpr:
            Condition(node->GetChild(0));
            Traversal(node->GetChild(1));
            if (node->GetChild(2)) CheckRule(node->GetChild(2));
            break;
        case RuleType::FuncDeclaration:
            _currentBlock = &(_currentBlock->AddBlock());
            FunctionDeclaration(node);
            _currentBlock = _currentBlock->upperBlock;
            break;
        case RuleType::FuncInvoke:
            FunctionInvoke(node);
            break;
        case RuleType::Return:
            ReturnExpression(node);
            break;
        case RuleType::InternalFuncInvoke:
            InternalFunctionInvoke(node);
            break;
        case RuleType::Block:
            _currentBlock = &(_currentBlock->AddBlock());
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
        if (node->GetChild(1) != nullptr)
            VariableAssignment(node->GetChild(0)->GetChild(1), node->GetChild(1));
    }
}

std::vector<ID_Data> SemanticAnalyzer::GroupLetVarDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupVarDeclaration)
        throw Err::CriticalError("Expected group var declaration", node);

    std::vector<ID_Data> idList;
    const std::vector<Node*> &pats = node->GetChilds();
    for (const auto &pat : pats) {
        std::string id = pat->GetChild(1)->GetData()->token.GetValue();
        if (_currentBlock->idTable.Has(id) || _currentBlock->arrayTable.Has(id))
            throw Err::VariableExistingError(id, pat->GetChild(1));
        ID_Data data;
        data.isMutable = pat->GetChild(0) != nullptr;
        data.id = id;
        data.location = *(pat->GetChild(1)->GetData()->token.GetLocation());
        _currentBlock->idTable.AddToTable(data);
        idList.emplace_back(data);
    }

    return idList;
}

std::vector<TypeData> SemanticAnalyzer::GroupInit(std::vector<ID_Data> variables, Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupInit)
        throw Err::CriticalError("Expected group initialize", node);

    std::vector<std::pair<TypeData, bool>> types;
    std::vector<TypeData> resTypes;
    const std::vector<Node*> &childs = node->GetChilds();
    types.reserve(childs.size());
    for (const auto &child : childs) {
        types.emplace_back(Expr(child));
    }

    if (variables.size() != types.size())
        throw Err::GroupVariableCountError(std::to_string(variables.size()), node);

    for (int i = 0; i < types.size(); i++) {
        if (types[i].second)
            throw Err::CriticalError("It is impossible to assign array to ordinary variable", node->GetChild(i));
        _currentBlock->idTable.GetData(variables[i].id).type = types[i].first;
        _currentBlock->idTable.GetData(variables[i].id).isInitialized = true;
        resTypes.emplace_back(types[i].first);
    }

    return resTypes;
}

std::pair<TypeData, bool> SemanticAnalyzer::Expr(Node *const &node) {
    std::pair<TypeData, bool> type;

    switch (node->GetData()->ruleType) {
        case RuleType::LogicalExpression:
        case RuleType::BinaryExpression:
        case RuleType::BinaryCompExpression:
        case RuleType::UnaryExpession:
            type = BoolExpr(node);
            break;
        case RuleType::MemberExpression:
            type = std::make_pair(MemberExpr(node), false);
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
    data.location = *(node->GetChild(1)->GetData()->token.GetLocation());

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

std::pair<TypeData, bool> SemanticAnalyzer::BoolExpr(Node *const &node) {

    if (node->GetData()->ruleType != LogicalExpression && node->GetData()->ruleType != RuleType::BinaryExpression
        && node->GetData()->ruleType != BinaryCompExpression)
        return MinTerminal(node);

    std::pair<TypeData, bool> left = BoolExpr(node->GetChild(0));
    std::pair<TypeData, bool> right = BoolExpr(node->GetChild(1));
    std::pair<TypeData, bool> wrongType;
    unsigned int wrongChild;

    switch (node->GetData()->ruleType){
        case RuleType::LogicalExpression:
            if (left.first.type == Type::Bool && right.first.type == Type::Bool)
                return left;
            wrongType = (left.first.type != Type::Bool) ? (wrongChild = 0, left) : (wrongChild = 1, right);
            throw Err::TypeError("Bool", wrongType.first.ToString(), node->GetChild(wrongChild));
        case RuleType::BinaryCompExpression:
            if (left.first == right.first) {
                if (left.second)
                    throw Err::CriticalError("Cannot using array in binary expressions", node->GetChild(0));
                if (right.second)
                    throw Err::CriticalError("Cannot using array in binary expressions", node->GetChild(1));
                return std::make_pair(TypeData(Type::Bool), false);
            }

            throw Err::TypeError(left.first.ToString(), right.first.ToString(), node->GetChild(1));
        case RuleType::BinaryExpression:
            if (left.first.type == Type::Bool)
                throw Err::CriticalError("It is impossible to using bool type in Binary Expression", node->GetChild(0));
            if (right.first.type == Type::Bool)
                throw Err::CriticalError("It is impossible to using bool type in Binary Expression", node->GetChild(1));
            if (left.first == right.first) {
                if (left.second)
                    throw Err::CriticalError("Cannot using array in binary expressions", node->GetChild(0));
                if (right.second)
                    throw Err::CriticalError("Cannot using array in binary expressions", node->GetChild(1));
                return left;
            }
            throw Err::TypeError(left.first.ToString(), right.first.ToString(), node->GetChild(1));
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

    int indexValue;
    try {
        indexValue =CalculateConstUnsignedExpression(node->GetChild(1)->GetChild(0));
    }
    catch (SemanticError &err) {
        indexValue = 0;
    }

    if ((elemsTypes[0].type != Type::Integer && elemsTypes[0].type != Type::Unsigned) || indexValue < 0)
        throw Err::CriticalError("Expected unsigned type index", node->GetChild(1)->GetChild(0));

    return GetArr(id, node->GetChild(0)).type;
}

std::vector<TypeData> SemanticAnalyzer::ArrayElems(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::ArrayElems)
        throw Err::CriticalError("Expected array elements", node);

    std::vector<TypeData> elemsTypes;
    const std::vector<Node*> &childs = node->GetChilds();
    for (const auto &child : childs) {
        if (child == nullptr)
            continue;
        std::pair<TypeData, bool> type = Expr(child);
        if (type.second)
            throw Err::CriticalError("It is impossible to using array in array elements", child);
        elemsTypes.emplace_back(type.first);
    }

    for (unsigned int i = 1; i < elemsTypes.size(); i++) {
        if (elemsTypes[i] != elemsTypes[0])
            throw Err::CriticalError("Different types in array elements", node->GetChild(i));
    }

    return elemsTypes;
}

std::pair<TypeData, bool> SemanticAnalyzer::MinTerminal(Node *const &node) {
    if (node->GetData()->ruleType == RuleType::UnaryExpession)
        return MinTerminal(node->GetChild(0));

    if (node->GetData()->ruleType == RuleType::FuncInvoke)
        return std::make_pair(FunctionInvoke(node), false);

    if (node->GetData()->ruleType == RuleType::InternalFuncInvoke) {
        return InternalFunctionInvoke(node);
    }

    if (node->GetData()->ruleType == RuleType::MemberExpression) {
        return std::make_pair(MemberExpr(node), false);
    }

    if (node->GetData()->ruleType == RuleType::Identifier) {
        std::string id = node->GetData()->token.GetValue();
        if (HasArrInUpper(id, node)) {
            Array_Data array = GetArr(id, node);
            if (!array.isInitialized)
                throw Err::ArrayInitializationError(id, node);
            return std::make_pair(array.type, true);
        }
        if (!GetID(id, node).isInitialized)
            throw Err::VariableInitializationError(id, node);
        return std::make_pair(GetID(id, node).type, false);
    }

    if (node->GetData()->ruleType == RuleType::Literal) {
        return std::make_pair(TypeData(TypeData::ToType(node->GetData()->token.GetType())), false);
    }

    throw Err::CriticalError("Expected minimal terminal", node);
}

TypeData SemanticAnalyzer::FunctionInvoke(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::FuncInvoke)
        throw Err::CriticalError("Expected function invoke", node);

    std::string functionId = node->GetChild(0)->GetData()->token.GetValue();
    if (!_functionTable.Has(functionId))
        throw Err::FunctionNotExistingError(functionId, node);

    FunctionInvokeParams(functionId, node->GetChild(1));

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
    data.location = *(arrPat->GetChild(1)->GetData()->token.GetLocation());

    Node* typeNode = arrPat->GetChild(2)->GetChild(0);
    data.type.isReference = typeNode->GetChild(0) != nullptr;
    data.type.isMutable = typeNode->GetChild(1) != nullptr;
    data.type.type = TypeData::ToType(typeNode->GetChild(2)->GetData()->token.GetType());
    int count = CalculateConstUnsignedExpression(arrPat->GetChild(2)->GetChild(1));
    if (count < 0)
        throw Err::CriticalError("Array size must be unsigned", arrPat->GetChild(2)->GetChild(1));
    data.elementCount = count;
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
        throw Err::CriticalError("Critical error: can not assigment", node);
}

void SemanticAnalyzer::VariableAssignment(Node* const& idNode, Node *const &exprNode) {
    std::string id = idNode->GetData()->token.GetValue();

    if (!HasIDInUpper(id, idNode))
        throw Err::VariableNotExistingError(id, idNode);

    ID_Data &idData = GetID(id, idNode);
    if (!idData.isMutable && idData.isInitialized)
        throw Err::VariableImmutableError(id, idNode);

    std::pair<TypeData, bool> exprType = Expr(exprNode);
    if (exprType.first.type == Type::None)
        throw Err::CriticalError("Expected non-none type", exprNode);
    if (exprType.second)
        throw Err::CriticalError("It is impossible to assign array to ordinary variable", exprNode);

    if (idData.type.type == Type::None)
        idData.type = exprType.first;
    else if (idData.type != exprType.first)
        throw Err::TypeError(idData.type.ToString(), exprType.first.ToString(), exprNode);

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

    std::pair<TypeData, bool> exprType = Expr(exprNode);
    if (exprType.second)
        throw Err::CriticalError("It is impossible to assign array to ordinary variable", exprNode);

    if (arrData.type != exprType.first)
        throw Err::TypeError(arrData.type.ToString(), exprType.first.ToString(), exprNode);
}

std::vector<std::pair<TypeData, bool>> SemanticAnalyzer::FunctionInvokeParams(const std::string &funcId, Node *const &node) {
    const std::vector<Node*> &paramsNode = node->GetChilds();
    std::vector<std::pair<TypeData, bool>> paramTypes;

    for (const auto &param : paramsNode) {
        if (param == nullptr)
            continue;
        if (param->GetData()->ruleType == RuleType::ArrayArg)
            paramTypes.emplace_back(std::make_pair(CanAccessArray(param->GetChild(2)), true));
        else if (param->GetData()->ruleType == RuleType::Identifier) {
            if (_currentBlock->arrayTable.Has(param->GetData()->token.GetValue())) {
                if (!GetArr(param->GetData()->token.GetValue(), param).isMutable)
                    throw Err::CriticalError("Can not borrow immutable a array as mutable", param);
                paramTypes.emplace_back(std::make_pair(CanAccessArray(param), true));
            }
            else {
                paramTypes.emplace_back(std::make_pair(CanAccessIdentifier(param), false));
            }
        }
        else
            paramTypes.emplace_back(Expr(param));
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

        if (paramTypes[i].second) {
            Node* arrIdNode = node->GetChild(i)->GetData()->ruleType == RuleType::ArrayArg ?
                              node->GetChild(i)->GetChild(2) : node->GetChild(i);

            std::string arrId = arrIdNode->GetData()->token.GetValue();
            Array_Data* declArrData = reinterpret_cast<Array_Data*>(funcDefineParams[i]);
            Array_Data paramArrayData = GetArr(arrId, node->GetChild(i));
            if (declArrData->elementCount != paramArrayData.elementCount)
                throw Err::ArrayCountElementsError(std::to_string(declArrData->elementCount), std::to_string(paramArrayData.elementCount), node);
        }
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
    ProgramBlock<ID_Data, Array_Data> *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return true;
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    return false;
}

bool SemanticAnalyzer::HasArrInUpper(const std::string &id, Node *const &root) {
    ProgramBlock<ID_Data, Array_Data> *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return true;
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    return false;
}

ID_Data& SemanticAnalyzer::GetID(const std::string &id, Node* const &root) {
    ProgramBlock<ID_Data, Array_Data> *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return tmp->idTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp);

    throw Err::VariableNotExistingError(id, root);
}

Array_Data& SemanticAnalyzer::GetArr(const std::string &id, Node* const &root) {
    ProgramBlock<ID_Data, Array_Data> *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return tmp->arrayTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp);

    throw Err::VariableNotExistingError(id, root);
}

void SemanticAnalyzer::Condition(Node *const &node) {
    std::pair<TypeData, bool> conditionType = BoolExpr(node);
    if (conditionType.first.type != Type::Bool)
        throw Err::TypeError("Bool", conditionType.first.ToString(), node);
}

void SemanticAnalyzer::FunctionDeclaration(Node *const &node) {
    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    if (_functionTable.Has(id)) {
        throw Err::FunctionExistingError(id, node->GetChild(0));
    }

    const std::vector<Node*> &arguments = node->GetChild(1)->GetChilds();

    Function_Data functionData;
    functionData.id = id;
    functionData.location = *(node->GetChild(0)->GetData()->token.GetLocation());

    for (const auto &argument : arguments) {
        if (argument == nullptr)
            continue;
        std::string varId = argument->GetChild(2)->GetData()->token.GetValue();
        if (_currentBlock->idTable.Has(varId) || _currentBlock->arrayTable.Has(varId)) {
            throw Err::VariableExistingError(varId, argument->GetChild(2));
        }

        if (argument->GetChild(3)->GetData()->ruleType == RuleType::IdType) {
            ID_Data idData = GetIDDefineParameter(argument);
            _currentBlock->idTable.AddToTable(idData);
            functionData.parameters.emplace_back(new ID_Data(idData));
        }
        else {
            Array_Data arrData = GetArrayDefineParameter(argument);
            _currentBlock->arrayTable.AddToTable(arrData);
            functionData.parameters.emplace_back(new Array_Data(arrData));
        }
    }

    functionData.type = GetTypeData(node->GetChild(2));
    _functionTable.AddToTable(functionData);
    Traversal(node->GetChild(3));
}

void SemanticAnalyzer::AddSystemFunctions() {
    _functionTable.AddToTable(Function_Data{"sqrt", TypeData(Type::Real)});
}

TypeData SemanticAnalyzer::GetTypeData(Node*const  &typeNode) {
    if (!typeNode)
        return TypeData();

    TypeData type;
    type.isReference = typeNode->GetChild(0) != nullptr;
    type.isMutable = typeNode->GetChild(1) != nullptr;
    type.type = TypeData::ToType(typeNode->GetChild(2)->GetData()->token.GetType());
    return type;
}

ID_Data SemanticAnalyzer::GetIDDefineParameter(Node *const &param) {
    ID_Data idData;
    idData.isInitialized = true;
    idData.isMutable = param->GetChild(1) != nullptr;
    idData.id = param->GetChild(2)->GetData()->token.GetValue();
    idData.location = *(param->GetChild(2)->GetData()->token.GetLocation());

    idData.type = GetTypeData(param->GetChild(3));
    return idData;
}

Array_Data SemanticAnalyzer::GetArrayDefineParameter(Node *const &param) {
    Array_Data arrayData;
    arrayData.isInitialized = true;
    arrayData.isMutable = param->GetChild(1) != nullptr;
    arrayData.id = param->GetChild(2)->GetData()->token.GetValue();
    arrayData.location = *(param->GetChild(2)->GetData()->token.GetLocation());

    arrayData.type = GetTypeData(param->GetChild(3)->GetChild(0));
    arrayData.elementCount = std::stoul(param->GetChild(3)->GetChild(1)->GetData()->token.GetValue());

    return arrayData;
}

void SemanticAnalyzer::ReturnExpression(Node* const &returnNode) {
    TypeData funcReturnType = _functionTable.Back().type;
    TypeData returnType = returnNode->GetChild(0) != nullptr ? Expr(returnNode->GetChild(0)).first : TypeData();

    if (funcReturnType != returnType)
        throw Err::FunctionReturnTypeError(funcReturnType.ToString(), returnType.ToString(), returnNode);
}

const Table<Function_Data> &SemanticAnalyzer::GetFunctionTable() const {
    return _functionTable;
}

void SemanticAnalyzer::CheckInits(const ProgramBlock<ID_Data, Array_Data> &programmBlock) {

    for (const auto& block : programmBlock.internalBlocks) {
        for (const auto &idData : block.idTable.GetTable()) {
            if (idData.type.type == Type::None)
                throw Err::VariableTypeNotExistingError(idData.id, idData.location);
        }
        for (const auto &arrData : block.arrayTable.GetTable()) {
            if (arrData.type.type == Type::None)
                throw Err::VariableTypeNotExistingError(arrData.id, arrData.location);
        }
        CheckInits(block);
    }

}

std::pair<TypeData, bool> SemanticAnalyzer::InternalFunctionInvoke(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::InternalFuncInvoke)
        Err::CriticalError("Expected internal function invoke", node);

    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    if (!HasIDInUpper(id, node))
        throw Err::VariableNotExistingError(id, node->GetChild(0));
    ID_Data idData = GetID(id, node);
    if (!idData.isInitialized)
        throw Err::VariableInitializationError(id, node->GetChild(0));
    if (idData.type.type != Type::Real)
        throw Err::CriticalError("Variable " + id + " should be real", node);

    return std::make_pair(FunctionInvoke(node->GetChild(1)), false);
}

int SemanticAnalyzer::CalculateConstUnsignedExpression(Node *const &node) {
    switch (node->GetData()->ruleType) {
        case RuleType::FuncInvoke:
        case RuleType::InternalFuncInvoke:
        case RuleType::Identifier:
        case RuleType::MemberExpression:
            throw Err::CriticalError("Can not using non-const value", node);
        case RuleType::UnaryExpession:
            throw Err::CriticalError("Unary expression can not be used", node);
        case RuleType::BinaryExpression:
            return BinaryOperation(CalculateConstUnsignedExpression(node->GetChild(0)),
                                   CalculateConstUnsignedExpression(node->GetChild(1)), node);
        case RuleType::Literal:
            if (node->GetData()->token.GetType() == TokenType::INTNUM)
                return std::stoi(node->GetData()->token.GetValue());
            throw Err::CriticalError("Only unsigned type can be used", node);
        default:
            throw Err::CriticalError("Can not calculated expression!", node);
    }
}

int SemanticAnalyzer::BinaryOperation(const int &left, const int &right, Node* const &operation) {
    if (operation == nullptr)
        throw std::exception();

    switch (operation->GetData()->token.GetType()) {
        case TokenType::PLUS:
            return left + right;
        case TokenType::MINUS:
            return left - right;
        case TokenType::MULT:
            return left * right;
        case TokenType::DIV:
            return left / right;
        case TokenType::MOD:
            return left % right;
        default:
            throw Err::CriticalError("Error in binary operation", operation);
    }
}