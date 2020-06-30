#ifndef RUSTCOMPILER_CODEGENERATOR_H
#define RUSTCOMPILER_CODEGENERATOR_H

#include <functional>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>
#include <stack>
#include <fstream>
#include "../Parser//AST_Tree/AST_Tree.h"
#include "../Semantics/ProgramBlock.h"
#include "../Exceptions/CodeGeneratorError.h"
#include "../Lexer/Token.h"
#include "MasmTypeData.h"

/// @file CodeGenerator.h

/// @brief класс генерации ассемблерного кода
class CodeGenerator {
private:
    union {
        uint32_t uint32;
        float f32;
    } Converter32{};

    union {
        uint64_t uint64;
        double f64;
    } Converter64{};

    enum RulesForLabels {
        While = 0, Loop, If, Else, Block,
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
    std::ifstream _tableOfReservedWords;
    Table<Function_Data> _funcTable;
    ProgramBlock<MasmID_Data, MasmArray_Data>* _currentBlock;
    std::string _template;
    std::string _asmCodeString;
    std::vector<MASMCompareOperation> _compareOperations;
    std::string _functionEndLabel;
    unsigned int _labelNum;
    std::stack<std::string> _breakLabels;
    std::stack<std::string> _ifEndLabels;
    std::stack<RulesForLabels> _rules;
    std::vector<int> _rulesCount;

    std::string Traversal(Node* const &root);
    std::string CheckRule(Node* const &node, const std::string &exitLabel = "");

    void InitCompareOperations();

    std::string FunctionDeclaration(Node* const &node);
    std::string ReturnExpression(Node* const &node);
    std::string FunctionParams(Node* const &node);
    std::string VariableDeclaration(Node* const &node);
    std::string ArrayDeclaration(Node* const &node);
    std::string GroupVariableDeclaration(Node* const &node);
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
    std::string GetLocalVariables(const ProgramBlock<MasmID_Data, MasmArray_Data> &programBlock);
    std::string TryOptimizedWithPush(Node* const &node, const MASMType &type);
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
    std::string GetCompareOperation(const TokenType &operation, const CompareType &compareType, const MASMType &type);

    bool HasIDInUpper(const std::string &id);
    bool HasArrInUpper(const std::string &id);
    bool HasInReservedWordsTable(std::string identifier);

    float BinaryOperation(float v1, float v2, Node* const &operation);
    float BinaryCompOperation(float v1, float v2, Node* const &operation);
    float LogicalCompOperation(float v1, float v2, Node* const &operation);
    float Optimized(Node* const &node);

    uint32_t FloatToHex(float value);
    uint64_t DoubleToHex(double value);

    std::pair<MASMType, std::string> DetermineType(Node *const &node);
    std::pair<bool, std::string> GetUID(ProgramBlock<MasmID_Data, MasmArray_Data>* const &block, const std::string &id);
    std::pair<std::string, std::string> PushReal8(Node* const &node);
    std::pair<std::string, std::string> PushReal8(double value);
    std::pair<bool, unsigned int> HasInUpper(const std::string &id);
    std::pair<MASMType, std::string> Type(Node* const &typeNode);

    MasmID_Data& GetID(const std::string &id);
    MasmArray_Data& GetArr(const std::string &id);

public:
    /// @param tree абстрактное дерево синтаксического разбора, полученное на этапе синтаксического анализа
    /// @param funcTable таблица функций, полученная на этапе семантического разбора
    explicit CodeGenerator(const AST_Tree &tree, const Table<Function_Data> &funcTable);
    ~CodeGenerator();
    /// @brief запуск генерации кода
    void Generate();
    /// @brief создание ассемблерного файла
    /// @note создает файл с именем temp.asm в подкаталоге TempCompile
    void CreateAsmFile();
};

#endif //RUSTCOMPILER_CODEGENERATOR_H
