#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(const AST_Tree &tree) {
    _tree = tree;
    _currentBlock = nullptr;
    _template = ".386\n"
                ".model flat, stdcall\n"
                ".stack 4096h\n"
                "\n"
                "include \\masm32\\include\\masm32rt.inc\n"
                "\n"
                ".code\n";
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
            _currentBlock = &(_currentBlock->AddBlock());
            rule = FunctionDeclaration(node);
            _localVariables.clear();
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

std::string CodeGenerator::FunctionDeclaration(Node* const &node) {
    std::string id = node->GetChild(0)->GetData()->token.GetValue();
    std::string params = FunctionParams(node->GetChild(1));
    std::string block = Traversal(node->GetChild(3));

    std::string function = id + " PROC " + params + "\n" + _localVariables + "\n" + block + "\n" + id + " ENDP";
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
            MasmID_Data idData(id, "", type.first);
            _currentBlock->idTable.AddToTable(idData);
        }
        else {
            type = Type(argument->GetChild(3)->GetChild(0));
            MasmArray_Data arrData(id, "", type.first);
            _currentBlock->arrayTable.AddToTable(arrData);
        }
        paramsCode += id + ": " + type.second;
    }

    return paramsCode;
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

std::string CodeGenerator::VeriableDeclaration(Node* const & node) {
    std::string code;

    Node* pat = node->GetChild(0);
    std::string id = pat->GetChild(1)->GetData()->token.GetValue();
    std::pair<MASMType, std::string> type = Type(pat->GetChild(2));
    std::string uid;

    if (HasIDInUpper(id)) {
        MasmID_Data prev = GetID(id);
        uid = prev.uid + "_";
    }

    MasmID_Data idData(id, uid, type.first);

    if (node->GetChild(1) != nullptr) {
        try {
            float value = Optimized(node->GetChild(1));
            idData.value = value;
            code += Assignment(id + uid, type.first, value);
        }
        catch (std::exception &err) {
            std::cout << "Can't optimized!\n";
            // TODO masm calculated
        }
        idData.isInitialize = true;
    }

    _currentBlock->idTable.AddToTable(idData);
    _localVariables += "\tLOCAL " + id + uid + ": " + type.second + "\n";

    return code;
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
    throw std::bad_function_call();
}

MasmArray_Data& CodeGenerator::GetArr(const std::string &id) {
    ProgramBlock<MasmID_Data, MasmArray_Data> *tmp = _currentBlock;

    do {
        if (tmp->arrayTable.Has(id))
            return tmp->arrayTable.GetData(id);
        tmp = tmp->upperBlock;
    } while (tmp->upperBlock);

    throw std::bad_function_call();
}

float CodeGenerator::Optimized(Node *const &node) {
    if (node == nullptr)
        return false;

    std::string id;
    std::string funcId;
    float ind;
    switch (node->GetData()->ruleType) {
        case RuleType::UnaryExpession:
            return Optimized(node->GetChild(0));
        case RuleType::BinaryExpression:
            return BinaryOperation(Optimized(node->GetChild(0)), Optimized(node->GetChild(1)), node);
        case RuleType::Identifier:
            id = node->GetData()->token.GetValue();
            if (HasIDInUpper(id)) {
                MasmID_Data idData = GetID(id);
                if (idData.isInitialize)
                    return GetID(id).value;
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
                    return sqrt(idData.value);
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
            return fmod(v1,v2);
        default:
            throw std::exception();
    }
}

uint32_t CodeGenerator::FloatToHex(float value) {
    Converter32.d = value;
    return Converter32.uu;
}

std::string CodeGenerator::Assignment(std::string id, const MASMType &type, float value) {
    std::string code;
    bool isNegative = value < 0;
    switch (type) {
        case MASMType::DWORD:
            code += "\tmov eax, " + std::to_string(static_cast<int>(value)) + "\n";
            code += "\tmov " + id + ", eax";
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
