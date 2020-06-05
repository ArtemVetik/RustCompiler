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
    for (const auto &child : childs) {
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
    } while (tmp);

    std::cout << "ERR ID: " << id << std::endl;
    throw std::bad_function_call();
}

MasmArray_Data& CodeGenerator::GetArr(const std::string &id) {
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
    for (const auto& argument : arguments) {
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

std::string CodeGenerator::VeriableDeclaration(Node* const &node) {
    std::string code;
    std::string uid;

    std::pair<MASMType, std::string> type;
    Node* pat = node->GetChild(0);
    Node* typeNode = pat->GetChild(2);
    std::string id = pat->GetChild(1)->GetData()->token.GetValue();
    if (typeNode == nullptr)
        type = DetermineType(node->GetChild(1));
    else
        type = Type(typeNode);

    if (HasIDInUpper(id)) {
        MasmID_Data prev = GetID(id);
        uid = prev.uid + "_";
    }

    MasmID_Data idData(id, uid, type.first, type.second);

    if (node->GetChild(1) != nullptr) {
        try {
            float value = Optimized(node->GetChild(1));
            idData.value = value;
            idData.isInitialize = true;
            code += Assignment(id + uid, type.first, value);
        }
        catch (std::exception &err) {
            code += CalculateExpression(node->GetChild(1), type.first);
            code += Assignment(idData.id + idData.uid, type.first);
        }
    }

    _currentBlock->idTable.AddToTable(idData);
    return code;
}

float CodeGenerator::Optimized(Node *const &node) {
    std::string id;
    std::string funcId;
    float ind;

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
            id = node->GetChild(0)->GetData()->token.GetValue();
            ind = Optimized(node->GetChild(1)->GetChild(0));
            if (HasArrInUpper(id)) {
                MasmArray_Data arrdata = GetArr(id);
                if (arrdata.isInitialize)
                    return arrdata.value[static_cast<int>(ind)];
            }
            throw std::bad_function_call();
        case RuleType::InternalFuncInvoke:
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

std::string CodeGenerator::Assignment(const std::string &id, const MASMType &type) {
    if (type == MASMType::DWORD)
        return "\tpop eax\n\tmov " + id + ", eax\n";
    else if (type == MASMType::REAL8)
        return "\tFSTP " + id + "\n";

    throw std::bad_function_call();
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
            return (tokenType == TokenType::INTNUM || tokenType == TokenType::BOOL) ? std::make_pair(MASMType::DWORD, "DWORD") : std::make_pair(MASMType::REAL8, "REAL8");
        default:
            return std::make_pair(MASMType::None, "None");
    }
}

std::string CodeGenerator::GetLocalVariables(const ProgramBlock<MasmID_Data, MasmArray_Data> &programmBlock) {
    std::string variables;

    for (const auto& block : programmBlock.internalBlocks) {
        for (const auto& idData : block.idTable.GetTable()) {
            variables += "\tLOCAL " + idData.id + idData.uid + ": " + idData.typeStr + "\n";
        }
        for (const auto &arrData : block.arrayTable.GetTable()) {
            variables += "\tLOCAL " + arrData.id + arrData.uid + "[" + std::to_string(arrData.elementCount) + "]: " + arrData.typeStr + "\n";
        }
        variables += GetLocalVariables(block);
    }
    return variables;
}

std::string CodeGenerator::CalculateExpression(Node *const &node, const MASMType &type) {
    std::string code;
    std::string funcId;
    MasmID_Data idData;

    switch (node->GetData()->ruleType) {
        case RuleType::UnaryExpession:
            code += UnaryOperation(node, type);
            break;
        case RuleType::BinaryExpression:
            code += CalculateExpression(node->GetChild(0), type);
            code += CalculateExpression(node->GetChild(1), type);
            code += BinaryOperation(node, type);
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
        default:
            throw std::bad_function_call();
    }

    return code;
}

std::string CodeGenerator::BinaryOperation(Node *const &operation, const MASMType &type) { // TODO REVIEW
    std::string code;

    switch (operation->GetData()->token.GetType()) {
        case TokenType::PLUS:
            if (type == MASMType::DWORD)
                code += "\tpop ebx\n\tpop eax\n\tadd eax, ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8)
                code += "\tFADD\n";
            break;
        case TokenType::MINUS:
            if (type == MASMType::DWORD)
                code += "\tpop ebx\n\tpop eax\n\tsub eax, ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8)
                code += "\tFXCH\n\tFSUB\n";
            break;
        case TokenType::MULT:
            if (type == MASMType::DWORD)
                code += "\tpop ebx\n\tpop eax\n\tmul ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8)
                code += "\tFMUL\n";
            break;
        case TokenType::DIV:
            if (type == MASMType::DWORD)
                code += "\tpop ebx\n\tpop eax\n\txor edx, edx\n\tidiv ebx\n\tpush eax\n";
            else if (type == MASMType::REAL8)
                code += "\tFXCH\n\tFDIV\n";
            break;
        case TokenType::MOD:
            if (type == MASMType::DWORD)
                code += "\tpop ebx\n\tpop eax\n\txor edx, edx\n\tidiv eax, ebx\n\tpush edx\n";
            else if (type == MASMType::REAL8)
                code += "\tFXCH\n\tFPREM\n";
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

    switch(node->GetData()->token.GetType()) {
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
    MASMType type = GetID(id).type;

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
    MasmArray_Data array = GetArr(node->GetChild(0)->GetData()->token.GetValue());

    code += CalculateExpression(node->GetChild(1)->GetChild(0), MASMType::DWORD);
    code += "\tpop ebx\n";

    if (type == MASMType::DWORD)
        code += "\tpush " + array.id + array.uid + "[ebx * Type " + array.id + array.uid + "]\n";
    else if (type == MASMType::REAL8)
        code += "\tFLD " + array.id + array.uid + "[ebx * Type " + array.id + array.uid + "]\n";

    return code;
}
