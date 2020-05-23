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
        case BinaryExpression:break;
        case LogicalExpression:break;
        case UnaryExpession:break;
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
        std::vector<std::string> varList = GroupLetVarDeclaration(node->GetChild(0));
        if (node->GetChild(1) != nullptr) {
            std::vector<TypeData> types = GroupInit(node->GetChild(1));
            if (varList.size() != types.size())
                throw SemanticError("Mismatch count of initialization parameters",
                        node->GetChild(1)->GetChild(types.size()-1)->GetData()->token.GetLocation());
            for (int i = 0; i < types.size(); i++) {
                _idTable.GetData(varList[i]).type = types[i];
                _idTable.GetData(varList[i]).isInitialized = true;
            }
        }
    }
    else {
        TypeData patType = Pat(node->GetChild(0));
        if (node->GetChild(1) != nullptr) {
            std::vector<TypeData> exprTypes = Expr(node->GetChild(1));
            if (node->GetChild(1)->GetData()->ruleType == RuleType::ArrayElems)
                throw SemanticError("Variable declaration: expected expression", node->GetChild(1)->GetData()->token.GetLocation());
            std::string id = node->GetChild(0)->GetChild(1)->GetData()->token.GetValue();
            _idTable.GetData(id).isInitialized = true;
            if (patType.type == Type::None) {
                _idTable.GetData(id).type = exprTypes[0];
                return;
            }
            if (patType != exprTypes[0])
                throw SemanticError(std::string("Type mismatch. Expected " + patType.ToString()
                + ", found " + exprTypes[0].ToString()), node->GetChild(1)->GetData()->token.GetLocation());
        }
    }
}

std::vector<std::string> SemanticAnalyzer::GroupLetVarDeclaration(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupVarDeclaration)
        throw SemanticError("Expected group var declaration", node->GetData()->token.GetLocation());

    std::vector<std::string> idList;
    std::vector<Node*> pats = node->GetChilds();
    for (const auto &pat : pats) {
        std::string id = pat->GetChild(1)->GetData()->token.GetValue();
        if (_idTable.Has(id) || _arrayTable.Has(id))
            throw SemanticError(std::string("Variable " + id + " already exists"), pat->GetChild(1)->GetData()->token.GetLocation());
        ID_Data data;
        data.isMutable = pat->GetChild(0) != nullptr;
        data.id = id;
        _idTable.AddToTable(data);
        idList.emplace_back(id);
    }

    return idList;
}

std::vector<TypeData> SemanticAnalyzer::GroupInit(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::GroupInit)
        throw SemanticError("Expected group init", node->GetData()->token.GetLocation());

    std::vector<TypeData> types;
    std::vector<Node*> childs = node->GetChilds();
    for (const auto &child : childs) {
        std::vector<TypeData> exprTypes = Expr(child);
        if (child->GetData()->ruleType == RuleType::ArrayElems)
            throw SemanticError("Group init expected single type for each expression", child->GetData()->token.GetLocation());
        types.emplace_back(exprTypes[0]);
    }

    return types;
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

    if (_idTable.Has(data.id) || _arrayTable.Has(data.id))
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
    unsigned int wrongChild;

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
        std::vector<TypeData> types = Expr(child);
        if (child->GetData()->ruleType == RuleType::ArrayElems)
            throw SemanticError("Array elements [] error", child->GetData()->token.GetLocation());
        elemsTypes.emplace_back(types[0]);
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
        else if (param->GetData()->ruleType == RuleType::Identifier) {
            std::string id = param->GetChild(0)->GetData()->token.GetValue();
            if (!_idTable.Has(id))
                throw SemanticError(std::string("Variable " + id + " has not been declared"), param->GetChild(2)->GetData()->token.GetLocation());
            paramTypes.emplace_back(_idTable.GetData(id).type);
        }
        else {
            std::vector<TypeData> exprParam = Expr(param);
            if (param->GetData()->ruleType == RuleType::ArrayElems)
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
            throw SemanticError(std::string("Type mismatch. Expected " + funcDefineParams[i]->type.ToString()),
                                paramsNode[i]->GetData()->token.GetLocation());
    }

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

    if (node->GetChild(1) != nullptr) {
        std::vector<TypeData> types = ArrayElems(node->GetChild(1));
        if (data.elementCount != types.size())
            throw SemanticError(std::string("Mismatch count of array elements. Expected " + std::to_string(data.elementCount))
            + ", found " + std::to_string(types.size()), node->GetChild(1)->GetChild(types.size()-1)->GetData()->token.GetLocation());
        if (types[0] != data.type)
            throw SemanticError(std::string("Mismatch types. Expected " + data.type.ToString()
            + ", found " + types[0].ToString()), node->GetChild(1)->GetChild(0)->GetData()->token.GetLocation());
        _arrayTable.GetData(data.id).isInitialized = true;
    }
}

void SemanticAnalyzer::Assignment(Node *const &node) {
    if (node->GetData()->ruleType != RuleType::AssignmentExpression)
        throw SemanticError("Expected assignment expression", node->GetData()->token.GetLocation());

    if (node->GetChild(0)->GetData()->ruleType == RuleType::MemberExpression) {
        TypeData memberType = MemberExpr(node->GetChild(0));
        if (node->GetChild(1)->GetData()->ruleType == RuleType::ArrayElems)
            throw SemanticError("Expected single-type expression", node->GetChild(1)->GetData()->token.GetLocation());
        std::vector<TypeData> exprTypes = Expr(node->GetChild(1));
        std::string arrId = node->GetChild(0)->GetChild(0)->GetData()->token.GetValue();
        if (!_arrayTable.Has(arrId))
            throw SemanticError(std::string("Array " + arrId + " has not been declared"),
                                node->GetChild(0)->GetChild(0)->GetData()->token.GetLocation());
        Array_Data arrData = _arrayTable.GetData(arrId);
        if (!arrData.isMutable && arrData.isInitialized)
            throw SemanticError(std::string("Can not assign twice to immutable variable " + arrId),
                                node->GetChild(0)->GetChild(0)->GetData()->token.GetLocation());
        if (memberType != exprTypes[0])
            throw SemanticError(std::string("Mismatch types. Expected " + memberType.ToString()
                                            + ", found " + exprTypes[0].ToString()),
                                node->GetChild(1)->GetData()->token.GetLocation());
    }
    else if (node->GetChild(0)->GetData()->ruleType == RuleType::Identifier) {
        std::string id = node->GetChild(0)->GetData()->token.GetValue();
        if (_idTable.Has(id)) {
            ID_Data idData = _idTable.GetData(id);
            if (!idData.isMutable && idData.isInitialized)
                throw SemanticError(std::string("Can not assign twice to immutable variable " + id),
                                    node->GetChild(0)->GetData()->token.GetLocation());
            if (node->GetChild(1)->GetData()->ruleType == RuleType::ArrayElems)
                throw SemanticError("Expected single-type expression", node->GetChild(1)->GetData()->token.GetLocation());
            TypeData exprType = Expr(node->GetChild(1))[0];
            if (idData.type != exprType)
                throw SemanticError(std::string("Mismatch types. Expected " + idData.type.ToString()
                                                + ", found " + exprType.ToString()),
                                    node->GetChild(1)->GetData()->token.GetLocation());
            _idTable.GetData(id).isInitialized = true;
        }
        else if (_arrayTable.Has(id)) {
            Array_Data arrData = _arrayTable.GetData(id);
            if (!arrData.isMutable && arrData.isInitialized)
                throw SemanticError(std::string("Can not assign twice to immutable variable " + id),
                                    node->GetChild(0)->GetData()->token.GetLocation());
            std::vector<TypeData> exprTypes = ArrayElems(node->GetChild(1));
            if (arrData.elementCount != exprTypes.size())
                throw SemanticError(std::string("Mismatch count of array elements. Expected " + std::to_string(arrData.elementCount))
                + ", found " + std::to_string(exprTypes.size()), node->GetChild(1)->GetChild(exprTypes.size()-1)->GetData()->token.GetLocation());
            if (exprTypes[0] != arrData.type)
                throw SemanticError(std::string("Mismatch types. Expected " + arrData.type.ToString()
                + ", found " + exprTypes[0].ToString()), node->GetChild(1)->GetChild(0)->GetData()->token.GetLocation());
            _arrayTable.GetData(arrData.id).isInitialized = true;
        }
        else
            throw SemanticError(std::string("Variable " + id + " has not been declared"), node->GetChild(0)->GetData()->token.GetLocation());
    }
    else
        throw SemanticError("Critical error: can not assigment", node->GetData()->token.GetLocation());
}
