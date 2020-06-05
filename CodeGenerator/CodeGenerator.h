#ifndef RUSTCOMPILER_CODEGENERATOR_H
#define RUSTCOMPILER_CODEGENERATOR_H

#include <functional>
#include <cmath>
#include <sstream>
#include "../AST_Tree/AST_Tree.h"
#include "../Semantics/ProgramBlock.h"
#include "../Lexer/Token.h"
#include "MasmTypeData.h"

class CodeGenerator {
private:
    union {
        uint32_t uu;
        float d;
    } Converter32{};

    AST_Tree _tree;
    Table<Function_Data> _funcTable;
    ProgramBlock<MasmID_Data, MasmArray_Data>* _currentBlock;
    std::string _template;

    std::string Traversal(Node* const &root);
    std::string CheckRule(Node* const &node);

    std::string FunctionDeclaration(Node* const &node);
    std::string FunctionParams(Node* const &node);
    std::pair<MASMType, std::string> Type(Node* const &typeNode);
    std::string VeriableDeclaration(Node* const &node);

    bool HasIDInUpper(const std::string &id);
    bool HasArrInUpper(const std::string &id);
    MasmID_Data& GetID(const std::string &id);
    MasmArray_Data& GetArr(const std::string &id);
    float BinaryOperation(float v1, float v2, Node* const &operation);
    std::string BinaryOperation(Node* const &operation, const MASMType &type);
    std::string UnaryOperation(Node *const &operation, const MASMType &type);

    std::string Assignment(const std::string &id, const MASMType &type, const float &value);
    std::string Assignment(const std::string &id, const MASMType &type);
    float Optimized(Node* const &node);
    uint32_t FloatToHex(float value);
    std::string GetLocalVariables(const ProgramBlock<MasmID_Data, MasmArray_Data> &programBlock);
    std::pair<MASMType, std::string> DetermineType(Node *const &node);

    std::string CalculateExpression(Node *const &node, const MASMType &type);
    std::string CalculateLiteral(Node *const &node);
    std::string CalculateIdentifier(Node *const &node);
    std::string CalculateMemberExpression(Node *const &node, const MASMType &type);

public:
    explicit CodeGenerator(const AST_Tree &tree, const Table<Function_Data> &funcTable);
    void Generate();

};

#endif //RUSTCOMPILER_CODEGENERATOR_H
