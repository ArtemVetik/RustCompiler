#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(const AST_Tree &tree, const Table<Function_Data> &funcTable) : _labelNum(0), _rulesCount(5) {
    _tree = tree;
    _funcTable = funcTable;
    _currentBlock = nullptr;
    _template = "\n.386\n" // TODO добавить вывод времени (chrono)
                ".model flat, stdcall\n"
                ".stack 4096h\n"
                "\n"
                "include \\masm32\\include\\masm32rt.inc\n"
                "\n"
                ".code\n\tFINIT\n\n";

    InitCompareOperations();
}

void CodeGenerator::InitCompareOperations() {
    _compareOperations.emplace_back(MASMCompareOperation {TokenType::MORE, "ja", "jbe", "jg", "jle"});
    _compareOperations.emplace_back(MASMCompareOperation {TokenType::LESS, "jb", "jae", "jl", "jge"});
    _compareOperations.emplace_back(MASMCompareOperation {TokenType::ASMR, "jae", "jb", "jge", "jl"});
    _compareOperations.emplace_back(MASMCompareOperation {TokenType::ASLS, "jbe", "ja", "jle", "jg"});
    _compareOperations.emplace_back(MASMCompareOperation {TokenType::NASSIG, "jne", "je", "jne", "je"});
    _compareOperations.emplace_back(MASMCompareOperation {TokenType::EQUAL, "je", "jne", "je", "jne"});
}

std::string CodeGenerator::GetCompareOperation(const TokenType &operation, const CodeGenerator::CompareType &compareType, const MASMType &type) {
    auto foundOperation = std::find_if(_compareOperations.cbegin(), _compareOperations.cend(),
    [&operation](const MASMCompareOperation &compOperation)
    {
        return operation == compOperation.operation;
    });
    if (foundOperation == _compareOperations.cend())
        throw std::bad_function_call();

    switch (compareType) {
        case Reverse:
            return type == MASMType::DWORD ? foundOperation->reverseCompSigned : foundOperation->reverseCompUnsigned;
        case Direct:
            return type == MASMType::DWORD ? foundOperation->directCompSigned : foundOperation->directCompUnsigned;
        default:
            throw std::bad_function_call();
    }
}

uint32_t CodeGenerator::FloatToHex(float value) {
    Converter32.d = value;
    return Converter32.uu;
}

uint64_t CodeGenerator::DoubleToHex(double value) {
    Converter64.d = value;
    return Converter64.uu;
}

void CodeGenerator::Generate() {
    std::string code = _template + Traversal(_tree.GetRoot());
    std::cout << code << std::endl;
}

std::string CodeGenerator::Traversal(Node *const &root) {
    const std::vector<Node *> &childs = root->GetChilds();

    std::string code;
    std::string tmpCode;
    for (const auto &child : childs) {
        tmpCode = CheckRule(child);
        if (!tmpCode.empty())
            tmpCode += "\n";
        code += tmpCode;
    }
    return code;
}

std::string CodeGenerator::CheckRule(Node *const &node, const std::string &exitLabel) {
    if (node == nullptr)
        return "";

    std::string rule;
    unsigned int ind;

    switch (node->GetData()->ruleType) {
        case RuleType::VariableDeclaration:
            if (node->GetChild(0)->GetData()->ruleType == RuleType::GroupVarDeclaration)
                rule = GroupVariableDeclaration(node);
            else
                rule = VariableDeclaration(node);
            break;
        case RuleType::ArrayDeclaration:
            rule = ArrayDeclaration(node);
            break;
        case RuleType::IfExpr:
            rule = IfExpression(node);
            break;
        case RuleType::LoopExpr:
            _rules.push(RulesForLabels::Loop);
            _rulesCount[RulesForLabels::Loop]++;
            rule = LoopExpression(node);
            _rulesCount[RulesForLabels::Loop]--;
            _rules.pop();
            break;
        case RuleType::AssignmentExpression:
            if (node->GetChild(0)->GetData()->ruleType == RuleType::Identifier)
                rule = IDAssignment(node->GetChild(0)->GetData()->token.GetValue(), node->GetChild(1));
            else if (node->GetChild(0)->GetData()->ruleType == RuleType::MemberExpression)
                rule = MemberArrayAssignment(node->GetChild(0)->GetChild(0)->GetData()->token.GetValue(),
                                             node->GetChild(1), node->GetChild(0)->GetChild(1)->GetChild(0));
            break;
        case RuleType::WhileExpr:
            _rules.push(RulesForLabels::While);
            _rulesCount[RulesForLabels::While]++;
            rule = WhileExpression(node);
            _rulesCount[RulesForLabels::While]--;
            _rules.pop();
            break;
        case RuleType::Break:
            rule = "\tjmp " + _breakLabels.top() + "\n";
            break;
        case RuleType::FuncDeclaration:
            _currentBlock = new ProgramBlock<MasmID_Data, MasmArray_Data>();
            rule = FunctionDeclaration(node);
            ProgramBlock<MasmID_Data, MasmArray_Data>::DeleteBlock(_currentBlock);
            break;
        case RuleType::Print:
            rule = Print(node);
            break;
        case RuleType::FuncInvoke:
            break;
        case RuleType::Return:
            if (!_functionEndLabel.empty())
                rule = "\tjmp " + _functionEndLabel + "\n";
            break;
        case RuleType::Block:
            _rules.push(RulesForLabels::Block);
            _currentBlock = &(_currentBlock->AddBlock());
            rule = Traversal(node);
            _currentBlock = _currentBlock->upperBlock;
            _rules.pop();
            break;
    }

    if (!exitLabel.empty())
        rule += "\tjmp " + exitLabel + "\n";
    return rule;
}

float CodeGenerator::Optimized(Node *const &node) {
    std::string id;
    std::string funcId;
    float ind;
    bool insideCycle = _rulesCount[RulesForLabels::While] != 0 || _rulesCount[RulesForLabels::Loop] != 0;

    switch (node->GetData()->ruleType) {
        case RuleType::UnaryExpession:
            if (node->GetData()->token.GetType() == TokenType::MINUS)
                return -Optimized(node->GetChild(0));
            else if (node->GetData()->token.GetType() == TokenType::EXCL)
                return !(static_cast<bool>(Optimized(node->GetChild(0))));
            return Optimized(node->GetChild(0));
        case RuleType::BinaryExpression:
            return BinaryOperation(Optimized(node->GetChild(0)), Optimized(node->GetChild(1)), node);
        case RuleType::Identifier:
            if (insideCycle) throw std::bad_function_call();
            id = node->GetData()->token.GetValue();
            if (HasIDInUpper(id)) {
                MasmID_Data idData = GetID(id);
                if (idData.isInitialize)
                    return idData.value;
            }
            throw std::bad_function_call();
        case RuleType::Literal:
            if (node->GetData()->token.GetValue() == "true") return 1.0f;
            else if (node->GetData()->token.GetValue() == "false") return 0.0f;
            return std::stof(node->GetData()->token.GetValue());
        case RuleType::MemberExpression:
            if (insideCycle) throw std::bad_function_call();
            id = node->GetChild(0)->GetData()->token.GetValue();
            ind = Optimized(node->GetChild(1)->GetChild(0));
            if (HasArrInUpper(id)) {
                MasmArray_Data &arrdata = GetArr(id);
                if (arrdata.isInitialize)
                    return arrdata.values[static_cast<int>(ind)];
            }
            throw std::bad_function_call();
        case RuleType::InternalFuncInvoke:
            if (insideCycle) throw std::bad_function_call();
            funcId = node->GetChild(1)->GetChild(0)->GetData()->token.GetValue();
            if (funcId != "sqrt")
                throw std::bad_function_call();

            id = node->GetChild(0)->GetData()->token.GetValue();
            if (HasIDInUpper(id)) {
                MasmID_Data idData = GetID(id);
                if (idData.isInitialize)
                    return sqrtf(idData.value);
            }
            throw std::bad_function_call();
        default:
            throw std::bad_function_call();
    }
}

std::pair<MASMType, std::string> CodeGenerator::DetermineType(Node *const &node) {
    if (node == nullptr)
        return std::make_pair(MASMType::None, "None");

    enum Type realType;
    MASMType masmType;
    TokenType tokenType;
    switch (node->GetData()->ruleType) {
        case RuleType::LogicalExpression:
        case RuleType::BinaryCompExpression:
            return std::make_pair(MASMType::DWORD, "DWORD");
        case RuleType::BinaryExpression:
        case RuleType::UnaryExpession:
            return DetermineType(node->GetChild(0));
        case RuleType::Identifier:
            masmType = GetID(node->GetData()->token.GetValue()).type.first;
            return masmType == MASMType::DWORD ? std::make_pair(masmType, "DWORD") : std::make_pair(masmType, "REAL8");
        case RuleType::MemberExpression:
            masmType = GetArr(node->GetChild(0)->GetData()->token.GetValue()).type.first;
            return masmType == MASMType::DWORD ? std::make_pair(masmType, "DWORD") : std::make_pair(masmType, "REAL8");
        case RuleType::FuncInvoke:
            realType = _funcTable.GetData(node->GetData()->token.GetValue()).type.type;
            return realType == Type::Real ? std::make_pair(MASMType::REAL8, "REAL8") : std::make_pair(MASMType::DWORD,
                                                                                                      "DWORD");
        case RuleType::InternalFuncInvoke:
            realType = _funcTable.GetData(node->GetChild(1)->GetChild(0)->GetData()->token.GetValue()).type.type;
            return realType == Type::Real ? std::make_pair(MASMType::REAL8, "REAL8") : std::make_pair(MASMType::DWORD,
                                                                                                      "DWORD");
        case RuleType::Literal:
            tokenType = node->GetData()->token.GetType();
            return (tokenType == TokenType::INTNUM || tokenType == TokenType::BOOL) ? std::make_pair(MASMType::DWORD,
                                                                                                     "DWORD")
                                                                                    : std::make_pair(MASMType::REAL8,
                                                                                                     "REAL8");
        default:
            return std::make_pair(MASMType::None, "None");
    }
}

std::string CodeGenerator::CalculateExpression(Node *const &node, const MASMType &type) {
    std::string code;
    std::string leftCode;
    std::string rightCode;
    std::string funcId;
    MasmID_Data idData;
    RuleType leftRuleType;

    switch (node->GetData()->ruleType) {
        case RuleType::UnaryExpession:
            code += UnaryOperation(node, type);
            break;
        case RuleType::BinaryExpression:
            leftCode = CalculateExpression(node->GetChild(0), type);
            rightCode = CalculateExpression(node->GetChild(1), type);
            leftRuleType = node->GetChild(0)->GetData()->ruleType;
            if (leftRuleType == RuleType::UnaryExpession || leftRuleType == RuleType::BinaryExpression)
                code += leftCode + rightCode + BinaryOperation(node, type);
            else
                code += rightCode + leftCode + BinaryOperation(node, type, true);
            break;
        case RuleType::Identifier:
            code += CalculateIdentifier(node);
            break;
        case RuleType::Literal:
            code += CalculateLiteral(node);
            break;
        case RuleType::MemberExpression:
            code += CalculateMemberExpression(node, type);
            break;
        case RuleType::InternalFuncInvoke:
            funcId = node->GetChild(1)->GetChild(0)->GetData()->token.GetValue();
            if (funcId != "sqrt")
                throw std::bad_function_call();

            idData = GetID(node->GetChild(0)->GetData()->token.GetValue());
            code += "\tFLD " + idData.id + idData.uid + "\n" + "\tFSQRT\n";
            break;
        case RuleType::FuncInvoke:
            FunctionInvoke(node);
            break;
        default:
            throw std::bad_function_call();
    }

    return code;
}

std::string CodeGenerator::BinaryOperation(Node *const &operation, const MASMType &type, bool isReverce) {
    std::string code;
    std::string popTemplate= "\tpop ebx\n\tpop eax\n";
    TokenType opType = operation->GetData()->token.GetType();

    if (opType != TokenType::PLUS && opType != TokenType::MULT) {
        if (isReverce && type == MASMType::REAL8 && opType != TokenType::MOD)
            code += "\tFXCH\n";
        else if (isReverce && type == MASMType::DWORD)
            code += "\txchg eax, ebx\n";
    }

    switch (opType) {
        case TokenType::PLUS:
            if (type == MASMType::DWORD)
                code += popTemplate + "\tadd eax, ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8)
                code += "\tFADD\n";
            break;
        case TokenType::MINUS:
            if (type == MASMType::DWORD)
                    code += popTemplate + "\tsub eax, ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8) {
                code += "\tFSUB\n";
            }
            break;
        case TokenType::MULT:
            if (type == MASMType::DWORD)
                code += popTemplate + "\tmul ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8)
                code += "\tFMUL\n";
            break;
        case TokenType::DIV:
            if (type == MASMType::DWORD)
                code += popTemplate + "\txor edx, edx\n\tidiv ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8) {
                code += "\tFDIV\n";
            }
            break;
        case TokenType::MOD:
            if (type == MASMType::DWORD)
                code += popTemplate + "\txor edx, edx\n\tidiv ebx\n\tpush edx\n";
            else if (type == MASMType::REAL8) {
                code += "\tFPREM\n\tFSTP st(1)\n";
            }
            break;
        default:
            throw std::bad_function_call();
    }

    return code;
}

std::string CodeGenerator::UnaryOperation(Node *const &operation, const MASMType &type) {
    std::string code;
    code += CalculateExpression(operation->GetChild(0), type);

    if (operation->GetData()->token.GetType() == TokenType::MINUS) {
        if (type == MASMType::DWORD)
            code += "\tpop eax\n\tNEG eax\n\tpush eax\n";
        else if (type == MASMType::REAL8)
            code += "\tFCHS\n";
    }

    return code;
}

std::string CodeGenerator::CalculateLiteral(Node *const &node) {
    std::string code;
    float value;

    switch (node->GetData()->token.GetType()) {
        case TokenType::INTNUM:
            code += "\tpush " + node->GetData()->token.GetValue() + "\n";
            break;
        case TokenType::RNUM:
            value = std::stof(node->GetData()->token.GetValue());
            std::stringstream stream;
            stream << std::hex << FloatToHex(value);
            std::string hexStr = (value < 0) ? "0" : "";
            hexStr += stream.str();
            code += "\tpush " + hexStr + "h" + "\n";
            code += "\tFLD DWORD PTR [esp]\n";
    }
    return code;
}

std::string CodeGenerator::CalculateIdentifier(Node *const &node) {
    std::string id = node->GetData()->token.GetValue();
    MASMType type = GetID(id).type.first;

    switch (type) {
        case MASMType::DWORD:
            return "\tpush " + id + "\n";
        case MASMType::REAL8:
            return "\tFLD " + id + "\n";
        default:
            throw std::bad_function_call();
    }
}

std::string CodeGenerator::CalculateMemberExpression(Node *const &node, const MASMType &type) {
    std::string code;
    const MasmArray_Data &array = GetArr(node->GetChild(0)->GetData()->token.GetValue());

    try {
        int ind = static_cast<int>(Optimized(node->GetChild(1)->GetChild(0)));
        code += "\tmov ebx, " + std::to_string(ind) + "\n";
    }
    catch (std::exception &err) {
        code += CalculateExpression(node->GetChild(1)->GetChild(0), MASMType::DWORD);
        code += "\tpop ebx\n";
    }

    std::string operation;
    if (array.isPtr) {
        operation = array.type.second + " ptr [esi + ebx * Type " + array.type.second + " ptr " + array.id + array.uid +
                    "]\n";
        code += "\tmov esi, " + array.id + array.uid + "\n";
    } else
        operation = array.id + array.uid + "[ebx * Type " + array.id + array.uid + "]\n";

    if (type == MASMType::DWORD) {
        code += "\tpush " + operation;
    } else if (type == MASMType::REAL8)
        code += "\tFLD " + operation;

    return code;
}

float CodeGenerator::BinaryOperation(float v1, float v2, Node *const &operation) {
    if (operation == nullptr)
        throw std::exception();

    switch (operation->GetData()->token.GetType()) {
        case TokenType::PLUS:
            return v1 + v2;
        case TokenType::MINUS:
            return v1 - v2;
        case TokenType::MULT:
            return v1 * v2;
        case TokenType::DIV:
            return v1 / v2;
        case TokenType::MOD:
            return std::fmod(v1, v2);
        default:
            throw std::exception();
    }
}

bool CodeGenerator::HasIDInUpper(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return true;
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    return false;
}

bool CodeGenerator::HasArrInUpper(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return true;
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    return false;
}

MasmID_Data &CodeGenerator::GetID(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return tmp->idTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp);

    std::cout << "ERR ID: " << id << std::endl;
    throw std::bad_function_call();
}

MasmArray_Data &CodeGenerator::GetArr(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return tmp->arrayTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp);

    std::cout << "ERR ARR: " << id << std::endl;
    throw std::bad_function_call();
}

std::pair<MASMType, std::string> CodeGenerator::Type(Node *const &typeNode) {
    Node *type = typeNode->GetChild(2);
    switch (type->GetData()->token.GetType()) {
        case TokenType::REAL:
            return std::make_pair(MASMType::REAL8, "REAL8");
        case TokenType::INTEGER:
        case TokenType::UINT:
            return std::make_pair(MASMType::DWORD, "DWORD");
        default:
            return std::make_pair(MASMType::None, "???");
    }
}

std::string CodeGenerator::FunctionDeclaration(Node *const &node) {
    _labelNum = 0;
    _functionEndLabel = "@M" + std::to_string(_labelNum++);

    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    std::string params = FunctionParams(node->GetChild(1));
    std::string block = CheckRule(node->GetChild(3));

    auto *tmp = _currentBlock;
    while (tmp->upperBlock)
        tmp = tmp->upperBlock;

    std::string localVariables = GetLocalVariables(*tmp);
    std::string funcEnd = (id == "main") ? "\tinkey\n\tcall ExitProcess\n" : "\tret\n";
    std::string function = id + " PROC " + params + "\n" + localVariables + block + _functionEndLabel + ":\n" + funcEnd +  id + " ENDP";
    if (id == "main") function += "\nEND " + id;

    function += "\n";

    _functionEndLabel.clear();
    return function;
}

std::string CodeGenerator::FunctionParams(Node *const &node) {
    const std::vector<Node *> &arguments = node->GetChilds();

    std::string paramsCode, ptrStr;
    std::string id;
    std::pair<MASMType, std::string> type;
    for (const auto &argument : arguments) {
        if (!paramsCode.empty())
            paramsCode += ", ";

        id = argument->GetChild(2)->GetData()->token.GetValue();
        if (argument->GetChild(3)->GetData()->ruleType == RuleType::IdType) {
            type = Type(argument->GetChild(3));
            MasmID_Data idData(id, "", type);
            _currentBlock->idTable.AddToTable(idData);
            ptrStr = "";
        } else {
            type = Type(argument->GetChild(3)->GetChild(0));
            MasmArray_Data arrData(id, "", type, 0, true);
            _currentBlock->arrayTable.AddToTable(arrData);
            ptrStr = "ptr ";
        }
        paramsCode += id + ": " + ptrStr + type.second;
    }

    return paramsCode;
}

std::string CodeGenerator::VariableDeclaration(Node *const &node) {
    std::string code;
    std::string uid;

    std::pair<MASMType, std::string> type;
    Node *pat = node->GetChild(0);
    Node *typeNode = pat->GetChild(2);
    std::string id = pat->GetChild(1)->GetData()->token.GetValue();

    if (typeNode == nullptr) {
        type = DetermineType(node->GetChild(1));
    } else {
        type = Type(typeNode);
    }

    if (HasIDInUpper(id))
        uid = GetID(id).uid + "_";
    else if (HasArrInUpper(id))
        uid = GetArr(id).uid + "_";

    MasmID_Data idData(id, uid, type);
    _currentBlock->idTable.AddToTable(idData);

    code += IDAssignment(id, node->GetChild(1));
    return code;
}

std::string CodeGenerator::IDAssignment(const std::string &id, Node *const &expression) {
    if (expression == nullptr)
        return "";

    if (expression->GetData()->ruleType == RuleType::ArrayElems)
        return ArrayAssignment(id, expression);

    std::string code;
    MasmID_Data &idData = GetID(id);

    std::pair<MASMType, std::string> type;
    if (idData.type.first == MASMType::None) {
        type = DetermineType(expression);
        idData.type = type;
    }
    else
        type = idData.type;

    if (expression) {
        try {
            float value = Optimized(expression);
            idData.value = value;
            idData.isInitialize = true;
            code += AssignmentByValue(idData.id + idData.uid, type.first, value);
        }
        catch (std::exception &err) {
            code += CalculateExpression(expression, type.first);
            code += AssignmentFromStack(idData.id + idData.uid, type.first);
        }
    }
    return code;
}

std::string CodeGenerator::ArrayDeclaration(Node *const &node) {
    std::string code;
    std::string uid;

    std::pair<MASMType, std::string> type;
    type = Type(node->GetChild(0)->GetChild(2)->GetChild(0));

    Node *countExpr = node->GetChild(0)->GetChild(2)->GetChild(1);
    auto count = static_cast<unsigned int>(Optimized(countExpr));

    std::string id = node->GetChild(0)->GetChild(1)->GetData()->token.GetValue();
    if (HasIDInUpper(id))
        uid = GetID(id).uid + "_";
    else if (HasArrInUpper(id))
        uid = GetArr(id).uid + "_";

    MasmArray_Data arrData(id, uid, type, count);
    _currentBlock->arrayTable.AddToTable(arrData);

    if (node->GetChild(1)->GetData()->ruleType == RuleType::ArrayElems)
        code += ArrayAssignment(id, node->GetChild(1));

    return code;
}

std::string CodeGenerator::ArrayAssignment(const std::string &id, Node *const &expression) {
    std::string code;

    const std::vector<Node *> &expressions = expression->GetChilds();

    for (int i = 0; i < expressions.size(); ++i) {
        code += MemberArrayAssignment(id, expressions[i], i);
    }
    GetArr(id).isInitialize = true;
    return code;
}

std::string
CodeGenerator::MemberArrayAssignment(const std::string &id, Node *const &expression, const unsigned int &ind) {
    std::string code;

    MasmArray_Data &arrData = GetArr(id);
    std::pair<MASMType, std::string> type = arrData.type;

    std::string idMasm, esiMember;
    if (arrData.isPtr) {
        esiMember = "\tmov esi, " + arrData.id + arrData.uid + "\n";
        idMasm = arrData.type.second + " ptr [esi + " + std::to_string(ind) + " * Type " + arrData.type.second +
                 " ptr " + arrData.id + arrData.uid + "]";
    } else
        idMasm = arrData.id + arrData.uid + "[" + std::to_string(ind) + " * Type " + arrData.id + arrData.uid + "]";

    if (expression) {
        try {
            float value = Optimized(expression);
            arrData.values[ind] = value;
            code += esiMember + AssignmentByValue(idMasm, type.first, value);
        }
        catch (std::exception &err) {
            code += CalculateExpression(expression, type.first);
            code += esiMember + AssignmentFromStack(idMasm, type.first);
        }
    }

    return code;
}

std::string CodeGenerator::MemberArrayAssignment(const std::string &id, Node *const &expression, Node *const &indNode) {
    std::string code;

    unsigned int ind;
    if (indNode) {
        try {
            ind = static_cast<unsigned int>(Optimized(indNode));
            code += MemberArrayAssignment(id, expression, ind);
        }
        catch (std::exception &err) {
            MasmArray_Data &arrData = GetArr(id);
            std::string fullName = arrData.id + arrData.uid;
            std::string memberExpr;
            code += CalculateExpression(indNode, MASMType::DWORD);
            if (arrData.isPtr) {
                memberExpr = "[esi + ebx * Type " + arrData.type.second + " ptr " + fullName + "]";
                code += "\tmov esi, " + fullName + "\n";
            }
            else
                memberExpr = fullName + "[ebx * Type " + fullName + "]";

            code += "\tpop ebx\n";
            code += AssignmentFromStack(memberExpr, arrData.type.first);
        }
    }

    return code;
}

std::string CodeGenerator::AssignmentByValue(const std::string &id, const MASMType &type, const float &value) {
    std::string code;
    std::stringstream stream;
    std::string hexStr;

    bool isNegative = value < 0;
    switch (type) {
        case MASMType::DWORD:
            code += "\tmov eax, " + std::to_string(static_cast<int>(value)) + "\n";
            code += "\tmov " + id + ", eax\n";
            break;
        case MASMType::REAL8:
            stream << std::hex << FloatToHex(value);
            hexStr = isNegative ? "0" : "";
            hexStr += stream.str();
            code += "\tpush " + hexStr + "h" + "\n";
            code += "\tFLD DWORD PTR [esp]\n";
            code += "\tFSTP " + id + "\n";
            break;
        default:
            throw std::bad_function_call();
    }

    return code;
}

std::string CodeGenerator::AssignmentFromStack(const std::string &id, const MASMType &type) {
    if (type == MASMType::DWORD)
        return "\tpop eax\n\tmov " + id + ", eax\n";
    else if (type == MASMType::REAL8)
        return "\tFSTP " + id + "\n";

    throw std::bad_function_call();
}

std::string CodeGenerator::GetLocalVariables(const ProgramBlock<MasmID_Data, MasmArray_Data> &programmBlock) {
    std::string variables;

    for (const auto &block : programmBlock.internalBlocks) {
        for (const auto &idData : block.idTable.GetTable()) {
            variables += "\tLOCAL " + idData.id + idData.uid + ": " + idData.type.second + "\n";
        }
        for (const auto &arrData : block.arrayTable.GetTable()) {
            variables += "\tLOCAL " + arrData.id + arrData.uid + "[" + std::to_string(arrData.elementCount) + "]: " +
                         arrData.type.second + "\n";
        }
        variables += GetLocalVariables(block);
    }
    if (!variables.empty()) variables += "\n";
    return variables;
}

std::string CodeGenerator::GroupVariableDeclaration(Node *const &node) {
    std::string code;
    std::string id;
    std::string uid;

    std::vector<Node *> variables = node->GetChild(0)->GetChilds();
    std::vector<Node *> expressions;

    std::pair<MASMType, std::string> type;

    if (node->GetChild(1) != nullptr) {
        expressions = node->GetChild(1)->GetChilds();
    }

    MasmID_Data idData;
    for (int i = 0; i < variables.size(); ++i) {
        id = variables[i]->GetChild(1)->GetData()->token.GetValue();
        if (HasIDInUpper(id))
            uid = GetID(id).uid + "_";
        else if (HasArrInUpper(id))
            uid = GetArr(id).uid + "_";

        if (expressions.empty())
            type = std::make_pair(MASMType::None, "None");
        else
            type = DetermineType(expressions[i]);

        idData = MasmID_Data(id, uid, type);
        _currentBlock->idTable.AddToTable(idData);

        code += IDAssignment(id, (expressions.empty()) ? nullptr : expressions[i]) + "\n";
    }

    return code;
}

std::string CodeGenerator::FunctionInvoke(Node *const &node) {
    std::string code;

    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    std::vector<Node *> params = node->GetChild(1)->GetChilds();

    //code += "\tInvoke " + id;

    for (const auto &param : params) {

    }

    return code;
}

std::string CodeGenerator::LogicalOperation(Node *const &operation, const CodeGenerator::CompareType &compare,
                                            const std::string &trueLabel, const std::string &falseLabel) {
    std::string code;
    std::string newLabel;

    switch (operation->GetData()->token.GetType()) {
        case TokenType::LOR:
            newLabel = "@M" + std::to_string(_labelNum++);
            code += LogicalOperation(operation->GetChild(0), CompareType::Direct, trueLabel, newLabel);
            code += newLabel + ":\n";
            code += LogicalOperation(operation->GetChild(1), compare, trueLabel, falseLabel);
            break;
        case TokenType::LAND:
            newLabel = "@M" + std::to_string(_labelNum++);
            code += LogicalOperation(operation->GetChild(0), CompareType::Reverse, newLabel, falseLabel);
            code += newLabel + ":\n";
            code += LogicalOperation(operation->GetChild(1), compare, trueLabel, falseLabel);
            break;
        case TokenType::MORE:
        case TokenType::LESS:
        case TokenType::ASMR:
        case TokenType::ASLS:
        case TokenType::NASSIG:
        case TokenType::EQUAL:
            code += CompareOperation(operation, compare, trueLabel, falseLabel);
            break;
    }

    return code;
}

std::string CodeGenerator::CompareOperation(Node* const &node, const CodeGenerator::CompareType &compare,
                            const std::string &trueLabel, const std::string &falseLabel) {
    std::string code;
    MASMType type = DetermineType(node->GetChild(0)).first;

    code += TryOptimizedWithPush(node->GetChild(0), type);
    code += TryOptimizedWithPush(node->GetChild(1), type);

    std::string compOperation = GetCompareOperation(node->GetData()->token.GetType(), compare, type);
    std::string label = (compare == CompareType::Direct) ? trueLabel : falseLabel;

    if (type == MASMType::DWORD)
        code += "\tpop ebx\n\tpop eax\n\tcmp eax, ebx\n";
    else
        code += "\tFXCH\n\tFCOMPP\n\tFSTSW ax\n\tSAHF\n";

    code += "\t" + compOperation + " " + label + "\n";

    return code;
}

std::string CodeGenerator::TryOptimizedWithPush(Node* const &node, const MASMType &type) {
    std::string code;

    try {
        float value = Optimized(node);
        if (type == MASMType::DWORD)
            code += "\tpush " + std::to_string(static_cast<int>(value)) + "\n";
        else code += "\tFLD FP8(" + std::to_string(value) + ")\n";
    }
    catch (std::exception & error) {
        code += CalculateExpression(node, type);
    }

    return code;
}

std::string CodeGenerator::IfExpression(Node *const &node) {
    std::string code;

    if (_rules.top() != RulesForLabels::Else) {
        _rules.push(RulesForLabels::If);
        _ifEndLabels.push("@M" + std::to_string(_labelNum++));
    }

    std::string trueLabel = "@M" + std::to_string(_labelNum++);
    std::string falseLabel = "@M" + std::to_string(_labelNum++);

    code += LogicalOperation(node->GetChild(0), CompareType::Reverse, trueLabel, falseLabel);

    code += trueLabel + ":\n";
    code += CheckRule(node->GetChild(1), _ifEndLabels.top());
    code += falseLabel + ":\n";

    _rules.push(RulesForLabels::Else);
    if (node->GetChild(2))
        code += CheckRule(node->GetChild(2));
    _rules.pop();

    if (_rules.top() != RulesForLabels::Else) {
        code += _ifEndLabels.top() + ":\n";
        _ifEndLabels.pop();
        _rules.pop();
    }

    return code;
}

std::string CodeGenerator::LoopExpression(Node *const &node) {
    std::string code;

    std::string label = "@M" + std::to_string(_labelNum++);
    _breakLabels.push("@M" + std::to_string(_labelNum++));

    code += label + ":\n";
    code += CheckRule(node->GetChild(0));
    code += "\tjmp " + label + "\n";
    code += _breakLabels.top() + ":\n";

    _breakLabels.pop();
    return code;
}

std::string CodeGenerator::WhileExpression(Node *const &node) {
    std::string code;

    std::string whileLabel = "@M" + std::to_string(_labelNum++);
    std::string blockLabel = "@M" + std::to_string(_labelNum++);
    _breakLabels.push("@M" + std::to_string(_labelNum++));

    code += whileLabel + ":\n";
    code += LogicalOperation(node->GetChild(0), CompareType::Reverse, blockLabel, _breakLabels.top());
    code += CheckRule(node->GetChild(1));
    code += "\tjmp " + whileLabel + "\n";
    code += _breakLabels.top() + ":\n";

    _breakLabels.pop();
    return code;
}

std::string CodeGenerator::Print(Node *const &node) {
    std::string code;
    std::stack<std::string> parameters;

    std::string fmtString = node->GetChild(0)->GetData()->token.GetValue();
    std::vector<Node*> exprList = node->GetChild(1)->GetChilds();

    for (const auto& expr : exprList) {
        if (expr->GetData()->ruleType == RuleType::Identifier) {
            if (HasIDInUpper(expr->GetData()->token.GetValue())) {
                MasmID_Data idData = GetID(expr->GetData()->token.GetValue());
                fmtString = FmtString(fmtString, idData.type.first);
                if (idData.type.first == MASMType::DWORD)
                    parameters.push("\tpush " + expr->GetData()->token.GetValue() + "\n");
                else if (idData.type.first == MASMType::REAL8) {
                    parameters.push("\tpush DWORD PTR " + idData.id + idData.uid + "\n");
                    parameters.push("\tpush DWORD PTR " + idData.id + idData.uid + " + 4\n");
                }

            }
            else if (HasArrInUpper(expr->GetData()->token.GetValue())) {
                MasmArray_Data arrayData = GetArr(expr->GetData()->token.GetValue());
                int posFormatPlace = fmtString.find("{:?}");
                code += CallPrintf(fmtString.substr(0, posFormatPlace) + "\"", parameters);
                parameters = std::stack<std::string>();
                code += PrintArray(arrayData);
                fmtString = "\"" + fmtString.substr(posFormatPlace+4, fmtString.size() - posFormatPlace - 4);
            }

        }
        else if (expr->GetData()->ruleType == RuleType::Literal) {
            MASMType type = (expr->GetData()->token.GetType() == TokenType::INTNUM) ? MASMType::DWORD : MASMType::REAL8;
            fmtString = FmtString(fmtString, type);
            if (type == MASMType::DWORD)
                parameters.push("\tpush " + expr->GetData()->token.GetValue() + "\n");
            else if (type == MASMType::REAL8) {
                std::pair<std::string, std::string> real8Pair = PushReal8(expr);
                parameters.push(real8Pair.second);
                parameters.push(real8Pair.first);
            }
        }
        else if (expr->GetData()->ruleType == RuleType::MemberExpression || expr->GetData()->ruleType == RuleType::InternalFuncInvoke ||
                 expr->GetData()->ruleType == RuleType::BinaryExpression || expr->GetData()->ruleType == RuleType::UnaryExpession) {
            MASMType type = DetermineType(expr).first;
            fmtString = FmtString(fmtString, type);
            try {
                float value = Optimized(expr);
                if (type == MASMType::DWORD)
                    parameters.push("\tpush " + std::to_string(static_cast<int>(value)) + "\n");
                else if (type == MASMType::REAL8) {
                    parameters.push("\tFLD FP8(" + std::to_string(value) + ")\n\tFSTP QWORD PTR [ebp]\n\tpush [ebp+4]\n\tpush [ebp]\n");
                    parameters.push("");
                }
            }
            catch (std::exception &err) {
                if (type == MASMType::DWORD)
                    parameters.push(CalculateExpression(expr, type));
                else if (type == MASMType::REAL8) {
                    parameters.push(CalculateExpression(expr, type) + "\tFSTP QWORD PTR [ebp]\n\tpush [ebp+4]\n\tpush [ebp]\n");
                    parameters.push("");
                }
            }
        }
    }

    code += CallPrintf(fmtString, parameters);
    return code;
}

std::string CodeGenerator::FmtString(const std::string &sourceStr, const MASMType &type) {
    std::string fmtString = sourceStr;
    int pos = sourceStr.find("{}");
    fmtString.replace(pos, 2, (type == MASMType::DWORD) ? "%d" : "%.2f");

    return fmtString;
}

std::string CodeGenerator::PrintArray(const MasmArray_Data &arrayData) {
    std::string code;
    std::string templatePrintfString = arrayData.type.second + " ptr " + arrayData.id + arrayData.uid +
                                                        "[ebx * Type " + arrayData.id + arrayData.uid + "]";

    if (arrayData.isPtr) {
        code += "\tmov esi, " + arrayData.id + arrayData.uid + "\n";
        templatePrintfString = arrayData.type.second + " ptr [esi + ebx * Type " + arrayData.id + arrayData.uid + "]";
    }
    code += "\tmov ebx, 0\n\tprintf(\"[\")\n";
    code += "\t.while (ebx < " + std::to_string(arrayData.elementCount) + ")\n";
    code += "\t\tpushad\n";
    code += "\t\tprintf(\"" + std::string(((arrayData.type.first == MASMType::DWORD) ? "%d " : "%.2f ")) + "\", " + templatePrintfString + ")\n";
    code += "\t\tpopad\n\t\tinc ebx\n\t.endw\n";
    code += "\tprintf(\"]\")\n";

    return code;
}

std::string CodeGenerator::CallPrintf(const std::string &fmtString, std::stack<std::string> parameters) {
    std::string code;

    code += "\tpushad\n";

    int paramSize = parameters.size();

    while (!parameters.empty()) {
        code += parameters.top();
        parameters.pop();
    }

    code += "\tprintf(" + fmtString + ")\n";

    while (paramSize > 0) {
        code += "\tpop eax\n";
        paramSize--;
    }

    code += "\tpopad\n";

    return code;
}

std::pair<std::string, std::string> CodeGenerator::PushReal8(Node* const &node) {
    std::pair<std::string, std::string> code;

    double value = std::stod(node->GetData()->token.GetValue());
    std::stringstream stream;
    stream << std::hex << DoubleToHex(value);
    std::string hexStr = (value < 0) ? "0" : "";
    code.first = "\tpush " + hexStr + stream.str().substr(0, 8) + "\n";
    code.second = "\tpush " + stream.str().substr(7, 8) + "\n";

    return code;
}