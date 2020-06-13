#ifndef RUSTCOMPILER_CODEGENERATOR_H
#define RUSTCOMPILER_CODEGENERATOR_H

#include <functional>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>
#include <stack>
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

    union {
        uint64_t uu;
        double d;
    } Converter64{};

    enum RulesForLabels {
        While = 0, Loop, If, Else, Block
    };

    enum CompareType {
        Reverse, Direct
    };

    struct MASMCompareOperation {
        TokenType operation;
        std::string directCompUnsigned;
        std::string reverseCompUnsigned;
        std::string directCompSigned;
        std::string reverseCompSigned;
    };

    AST_Tree _tree;
    Table<Function_Data> _funcTable;
    ProgramBlock<MasmID_Data, MasmArray_Data>* _currentBlock;
    std::string _template;
    std::vector<MASMCompareOperation> _compareOperations;
    std::string _functionEndLabel;
    unsigned int _labelNum;
    std::stack<std::string> _breakLabels;
    std::stack<std::string> _ifEndLabels;
    std::stack<RulesForLabels> _rules;
    std::vector<int> _rulesCount;

    std::string Traversal(Node* const &root);
    std::string CheckRule(Node* const &node, const std::string &exitLabel = "");

    std::string FunctionDeclaration(Node* const &node);
    std::string FunctionParams(Node* const &node);
    std::pair<MASMType, std::string> Type(Node* const &typeNode);
    std::string VariableDeclaration(Node* const &node);
    std::string ArrayDeclaration(Node* const &node);
    std::string GroupVariableDeclaration(Node* const &node);

    bool HasIDInUpper(const std::string &id);
    bool HasArrInUpper(const std::string &id);
    MasmID_Data& GetID(const std::string &id);
    MasmArray_Data& GetArr(const std::string &id);
    float BinaryOperation(float v1, float v2, Node* const &operation);
    std::string BinaryOperation(Node* const &operation, const MASMType &type, bool isReverce = false);
    std::string UnaryOperation(Node *const &operation, const MASMType &type);

    std::string LogicalOperation(Node* const &operation, const CompareType &compare, const std::string &trueLabel, const std::string &falseLabel);
    std::string CompareOperation(Node* const &operation,const CompareType &compare, const std::string &trueLabel, const std::string &falseLabel);

    std::string ArrayAssignment(const std::string &id, Node *const &expression);
    std::string MemberArrayAssignment(const std::string &id, Node *const &expression, const unsigned int &ind);
    std::string MemberArrayAssignment(const std::string &id, Node *const &expression, Node* const &indNode);
    std::string IDAssignment(const std::string &id, Node *const &expression);
    std::string AssignmentByValue(const std::string &id, const MASMType &type, const float &value);
    std::string AssignmentFromStack(const std::string &id, const MASMType &type);
    float Optimized(Node* const &node);
    std::string TryOptimizedWithPush(Node* const &node, const MASMType &type);
    uint32_t FloatToHex(float value);
    uint64_t DoubleToHex(double value);
    std::string GetLocalVariables(const ProgramBlock<MasmID_Data, MasmArray_Data> &programBlock);
    std::pair<MASMType, std::string> DetermineType(Node *const &node);

    std::string Print(Node* const &node);
    std::string FmtString(const std::string &sourceStr, const MASMType &type);
    std::string PrintArray(const MasmArray_Data &arrayData);
    std::string CallPrintf(const std::string &fmtString, std::stack<std::string> parameters);

    std::string IfExpression(Node *const &Node);
    std::string LoopExpression(Node *const &Node);
    std::string WhileExpression(Node *const &Node);

    std::string CalculateExpression(Node *const &expression, const MASMType &type);
    std::string CalculateLiteral(Node *const &node);
    std::string CalculateIdentifier(Node *const &node);
    std::string CalculateMemberExpression(Node *const &node, const MASMType &type);
    std::string FunctionInvoke(Node *const &pNode);
    std::pair<std::string, std::string> PushReal8(Node* const &node);

    std::string GetCompareOperation(const TokenType &operation, const CompareType &compareType, const MASMType &type);
    void InitCompareOperations();

public:
    explicit CodeGenerator(const AST_Tree &tree, const Table<Function_Data> &funcTable);

    void Generate();
};

#endif //RUSTCOMPILER_CODEGENERATOR_H