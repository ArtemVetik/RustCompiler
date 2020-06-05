#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(const AST_Tree &tree, const Table<Function_Data> &funcTable) {
    _tree = tree;
    _funcTable = funcTable;
    _currentBlock = nullptr;
    _template = ".386\n"
                ".model flat, stdcall\n"
                ".stack 4096h\n"
                "\n"
                "include \\masm32\\include\\masm32rt.inc\n"
                "\n"
                ".code\n\tFINIT\n\n";
}

void CodeGenerator::Generate() {
    std::string code = _template + Traversal(_tree.GetRoot());

    std::cout << code << std::endl;
}

std::string CodeGenerator::Traversal(Node *const &root) {
    const std::vector<Node*> &childs = root->GetChilds();

    std::string code;
    std::string tmpCode;
    for (const auto &child: childs) {
        tmpCode = CheckRule(child);
        if (!tmpCode.empty())
            tmpCode += "\n";
        code += tmpCode;
    }
    return code;
}

std::string CodeGenerator::CheckRule(Node *const &node) {
    if (node == nullptr)
        return "";

    std::string rule;

    switch (node->GetData()->ruleType) {
        case RuleType::VariableDeclaration:
            rule = VeriableDeclaration(node);
            break;
        case RuleType::ArrayDeclaration:
            break;
        case RuleType::AssignmentExpression:
            break;
        case RuleType::WhileExpr:
            break;
        case RuleType::IfExpr:
            break;
        case RuleType::FuncDeclaration:
            _currentBlock = new ProgramBlock<MasmID_Data, MasmArray_Data>();
            rule = FunctionDeclaration(node);
            ProgramBlock<MasmID_Data, MasmArray_Data>::DeleteBlock(_currentBlock);
            delete _currentBlock;
            break;
        case RuleType::FuncInvoke:
            break;
        case RuleType::Return:
            break;
        case RuleType::Block:
            _currentBlock = &(_currentBlock->AddBlock());
            rule = Traversal(node);
            _currentBlock = _currentBlock->upperBlock;
            break;
    }

    return rule;
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

MasmID_Data& CodeGenerator::GetID(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->idTable.Has(id))
            return tmp->idTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    std::cout << "ERR ID: " << id << std::endl;
    throw std::bad_function_call();
}

MasmArray_Data& CodeGenerator::GetArr(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return tmp->arrayTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    std::cout << "ERR ARR: " << id << std::endl;
    throw std::bad_function_call();
}

std::pair<MASMType, std::string> CodeGenerator::Type(Node *const &typeNode) {
    Node* type = typeNode->GetChild(2);
    switch (type->GetData()->token.GetType()) {
        case TokenType::REAL:
            return std::make_pair(MASMType::REAL8,"REAL8");
        case TokenType::INTEGER:
        case TokenType::UINT:
            return std::make_pair(MASMType::DWORD,"DWORD");
        default:
            return std::make_pair(MASMType::None,"???");
    }
}

std::string CodeGenerator::FunctionDeclaration(Node* const &node) {
    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    std::string params = FunctionParams(node->GetChild(1));
    std::string block = CheckRule(node->GetChild(3));

    auto *tmp = _currentBlock;
    while (tmp->upperBlock)
        tmp = tmp->upperBlock;

    std::string _localVariables = GetLocalVariables(*tmp);

    std::string function = id + " PROC " + params + "\n" + _localVariables + "\n" + block + id + " ENDP";
    if (id == "main") function += "\nEND " + id;

    function += "\n";
    return function;
}

std::string CodeGenerator::FunctionParams(Node *const &node) {
    const std::vector<Node*> &arguments = node->GetChilds();

    std::string paramsCode;
    std::string id;
    std::pair<MASMType, std::string> type;
    for (const auto &argument : arguments) {
        if (!paramsCode.empty())
            paramsCode += ", ";

        id = argument->GetChild(2)->GetData()->token.GetValue();
        if (argument->GetChild(3)->GetData()->ruleType == RuleType::IdType) {
            type = Type(argument->GetChild(3));
            MasmID_Data idData(id, "", type.first, type.second);
            _currentBlock->idTable.AddToTable(idData);
        }
        else {
            type = Type(argument->GetChild(3)->GetChild(0));
            MasmArray_Data arrData(id, "", type.first, type.second);
            _currentBlock->arrayTable.AddToTable(arrData);
        }
        paramsCode += id + ": " + type.second;
    }

    return paramsCode;
}

std::string CodeGenerator::VeriableDeclaration(Node* const & node) {
    std::string code;

    std::pair<MASMType, std::string> type;
    Node* pat = node->GetChild(0);
    Node* typeNode = pat->GetChild(2);
    std::string id = pat->GetChild(1)->GetData()->token.GetValue();
    if (typeNode == nullptr)
        type = DetermineType(node->GetChild(1));
    else
        type = Type(typeNode);
    std::string uid;

    if (HasIDInUpper(id)) {
        MasmID_Data prev = GetID(id);
        uid = prev.uid + "_";
    }

    MasmID_Data idData(id, uid, type.first, type.second);

    if (node->GetChild(1) != nullptr) {
        try {
            float value = Optimized(node->GetChild(1));
            idData.value = value;
            code += Assignment(id + uid, type.first, value);
        }
        catch (std::exception &err) {
            std::cout << "Can't optimized " + idData.id + idData.uid + "\n";
            // TODO masm calculated
        }
        idData.isInitialize = true;
    }

    _currentBlock->idTable.AddToTable(idData);
    return code;
}

float CodeGenerator::Optimized(Node *const &node) {
    if (node == nullptr)
        return false;
    
    std::string id;
    std::string funcId;
    float ind;
    switch (node->GetData()->ruleType) {
        case RuleType::UnaryExpession:
            return Optimized(node->GetChild(0)); // TODO исправить унарный минус в оптимизации
        case RuleType::BinaryExpression:
            return BinaryOperation(Optimized(node->GetChild(0)), Optimized(node->GetChild(1)), node);
        case RuleType::Identifier:
            id = node->GetData()->token.GetValue();
            if (HasIDInUpper(id)) {
                MasmID_Data idData = GetID(id);
                if (idData.isInitialize)
                    return idData.value;
            }
            throw std::exception();
        case RuleType::Literal:
            return std::stof(node->GetData()->token.GetValue());
        case RuleType::MemberExpression:
            id = node->GetChild(0)->GetData()->token.GetValue();
            ind = Optimized(node->GetChild(1)->GetChild(0));
            if (HasArrInUpper(id)) {
                MasmArray_Data arrdata = GetArr(id);
                if (arrdata.isInitialize)
                    return arrdata.value[static_cast<int>(ind)];
            }
            throw std::exception();
        case RuleType::InternalFuncInvoke:
            funcId = node->GetChild(1)->GetChild(0)->GetData()->token.GetValue();
            if (funcId != "sqrt")
                throw std::exception();

            id = node->GetChild(0)->GetData()->token.GetValue();
            if (HasIDInUpper(id)) {
                MasmID_Data idData = GetID(id);
                if (idData.isInitialize)
                    return std::sqrt(idData.value);
            }
            throw std::exception();
        default:
            throw std::exception();
    }
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
            return std::fmod(v1,v2);
        default:
            throw std::exception();
    }
}

uint32_t CodeGenerator::FloatToHex(float value) {
    Converter32.d = value;
    return Converter32.uu;
}

std::string CodeGenerator::Assignment(const std::string &id, const MASMType &type, const float &value) {
    std::string code;
    bool isNegative = value < 0;
    switch (type) {
        case MASMType::DWORD:
            code += "\tmov eax, " + std::to_string(static_cast<int>(value)) + "\n";
            code += "\tmov " + id + ", eax\n";
            break;
        case MASMType::REAL8:
            std::stringstream stream;
            stream << std::hex << FloatToHex(value);
            std::string hexStr = isNegative ? "0" : "";
            hexStr += stream.str();
            code += "\tpush " + hexStr + "h" + "\n";
            code += "\tFLD DWORD PTR [esp]\n";
            code += "\tFSTP " + id + "\n";
            break;
    }

    return code;
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
            masmType = GetID(node->GetData()->token.GetValue()).type;
            return masmType == MASMType::DWORD ? std::make_pair(masmType, "DWORD") : std::make_pair(masmType, "REAL8");
        case RuleType::MemberExpression:
            masmType = GetArr(node->GetData()->token.GetValue()).type;
            return masmType == MASMType::DWORD ? std::make_pair(masmType, "DWORD") : std::make_pair(masmType, "REAL8");
        case RuleType::FuncInvoke:
            realType = _funcTable.GetData(node->GetData()->token.GetValue()).type.type;
            return realType == Type::Real ? std::make_pair(MASMType::REAL8, "REAL8") : std::make_pair(MASMType::DWORD, "DWORD");
        case RuleType::InternalFuncInvoke:
            realType = _funcTable.GetData(node->GetChild(1)->GetChild(0)->GetData()->token.GetValue()).type.type;
            return realType == Type::Real ? std::make_pair(MASMType::REAL8, "REAL8") : std::make_pair(MASMType::DWORD, "DWORD");
        case RuleType::Literal:
            tokenType = node->GetData()->token.GetType();
            return tokenType == TokenType::INTNUM ? std::make_pair(MASMType::DWORD, "DWORD") : std::make_pair(MASMType::REAL8, "REAL8");
        default:
            return std::make_pair(MASMType::None, "None");
    }
}

std::string CodeGenerator::GetLocalVariables(const ProgramBlock<MasmID_Data, MasmArray_Data> &programmBlock) {
    std::string variables;

    for (const auto& block : programmBlock.internalBlocks) {
        for (const auto &idData : block.idTable.GetTable()) {
            variables += "\tLOCAL " + idData.id + idData.uid + ": " + idData.typeStr + "\n";
        }
        for (const auto &arrData : block.arrayTable.GetTable()) {
            variables += "\tLOCAL " + arrData.id + arrData.uid + "[" + std::to_string(arrData.elementCount) + "]: " + arrData.typeStr + "\n";
        }
        variables += GetLocalVariables(block);
    }
    return variables;
}