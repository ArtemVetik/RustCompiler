#ifndef RUSTCOMPILER_SEMANTICANALYZER_H
#define RUSTCOMPILER_SEMANTICANALYZER_H


#include "../Parser/AST_Tree/AST_Tree.h"
#include "SemanticErrorDebugger.h"
#include "ProgramBlock.h"

#define Err SemanticErrorDebugger

/// @brief семантический анализатор
class SemanticAnalyzer {
private:
    Node* _root;
    Table<Function_Data> _functionTable;
    ProgramBlock<ID_Data, Array_Data> *_currentBlock;

    void AddSystemFunctions();

    void Traversal(Node* const &root);
    void CheckRule(Node* const &node);

    void VariableDeclaration(Node* const &node);
    void ArrayDeclaration(Node* const &node);
    void FunctionDeclaration(Node* const &node);
    void Assignment(Node* const &node);
    void VariableAssignment(Node* const& idNode, Node *const &exprNode);
    void ArrayAssignment(Node* const& idNode, Node *const &exprNode);
    void MemberAssignment(Node* const& idNode, Node *const &exprNode);
    void Condition(Node* const &node);
    void CheckInits(const ProgramBlock<ID_Data, Array_Data> &pBlock);
    void ReturnExpression(Node* const &returnNode);
    void Print(Node* const &node);

    int CalculateConstUnsignedExpression(Node* const &node, const std::vector<RuleType> &forbiddenRule = std::vector<RuleType>());
    int BinaryOperation(const int &left, const int &right, Node* const &node);

    bool HasIDInUpper(const std::string &id, Node *const &root);
    bool HasArrInUpper(const std::string &id, Node *const &root);

    std::vector<ID_Data> GroupLetVarDeclaration(Node *const &node);
    std::vector<TypeData> GroupInit(std::vector<ID_Data> variables, Node* const &node);
    std::vector<TypeData> ArrayElems(Node* const &node);
    std::vector<std::pair<TypeData, bool>> FunctionInvokeParams(const std::string &funcId, Node* const &node);

    std::pair<TypeData, bool> Expr(Node* const &node);
    std::pair<TypeData, bool> BoolExpr(Node* const &node);
    std::pair<TypeData, bool> MinTerminal(Node* const &node);
    std::pair<TypeData, bool> InternalFunctionInvoke(Node* const &node);
    std::pair<bool, unsigned int> HasInUpper(const std::string &id);

    TypeData MemberExpr(Node* const &node);
    TypeData FunctionInvoke(Node* const &node);
    TypeData CanAccessIdentifier(Node* const &idNode);
    TypeData CanAccessArray(Node* const &idNode);
    TypeData GetTypeData(Node* const &typeNode);

    ID_Data GetIDDefineParameter(Node* const &param);
    ID_Data Pat(Node* const &node);
    Array_Data GetArrayDefineParameter(Node* const &param);
    ID_Data& GetID(const std::string &id, Node* const &root);
    Array_Data& GetArr(const std::string &id, Node* const &root);

public:
    /// @param tree абстрактное синтаксическое дерево, полученное в результате синтаксического анализа
    explicit SemanticAnalyzer(const AST_Tree &tree);
    ~SemanticAnalyzer();

    /// @brief запускает семантический анализ
    void Analyze();
    /// @return таблица функций, сформированная на этапе семантического анализа
    [[nodiscard]] const Table<Function_Data> &GetFunctionTable() const;
};


#endif //RUSTCOMPILER_SEMANTICANALYZER_H
