#ifndef RUSTCOMPILER_SEMANTICANALYZER_H
#define RUSTCOMPILER_SEMANTICANALYZER_H


#include "../AST_Tree/AST_Tree.h"
#include "SemanticErrorDebugger.h"
#include "ProgramBlock.h"

#define Err SemanticErrorDebugger

class SemanticAnalyzer {
private:
    Node* _root;
    Table<Function_Data> _functionTable;
    ProgramBlock<ID_Data, Array_Data> *_currentBlock;

    void AddSystemFunctions();

    void Traversal(Node* const &root);
    void CheckRule(Node* const &node);
    // TODO отформатировать порядок методов
    void VariableDeclaration(Node* const &node);
    void ArrayDeclaration(Node* const &node);
    void FunctionDeclaration(Node* const &node);
    std::vector<ID_Data> GroupLetVarDeclaration(Node *const &node);

    ID_Data Pat(Node* const &node);
    std::vector<TypeData> GroupInit(std::vector<ID_Data> variables, Node* const &node);

    void Condition(Node* const &node);
    std::pair<TypeData, bool> Expr(Node* const &node);
    std::pair<TypeData, bool> BoolExpr(Node* const &node);
    std::pair<TypeData, bool> MinTerminal(Node* const &node);
    TypeData MemberExpr(Node* const &node);
    TypeData FunctionInvoke(Node* const &node);

    std::vector<std::pair<TypeData, bool>> FunctionInvokeParams(const std::string &funcId, Node* const &node);
    std::pair<TypeData, bool> InternalFunctionInvoke(Node* const &node);

    void Assignment(Node* const &node);
    void VariableAssignment(Node* const& idNode, Node *const &exprNode);
    void ArrayAssignment(Node* const& idNode, Node *const &exprNode);
    void MemberAssignment(Node* const& idNode, Node *const &exprNode);
    std::vector<TypeData> ArrayElems(Node* const &node);

    void ReturnExpression(Node* const &returnNode);
    void Print(Node* const &node);

    int CalculateConstUnsignedExpression(Node* const &node);
    int BinaryOperation(const int &left, const int &right, Node* const &node);

    TypeData CanAccessIdentifier(Node* const &idNode);
    TypeData CanAccessArray(Node* const &idNode);
    TypeData GetTypeData(Node* const &typeNode);

    ID_Data GetIDDefineParameter(Node* const &param);
    Array_Data GetArrayDefineParameter(Node* const &param);
    bool HasIDInUpper(const std::string &id, Node *const &root);
    bool HasArrInUpper(const std::string &id, Node *const &root);
    ID_Data& GetID(const std::string &id, Node* const &root);
    Array_Data& GetArr(const std::string &id, Node* const &root);

    void CheckInits(const ProgramBlock<ID_Data, Array_Data> &pBlock);

public:
    explicit SemanticAnalyzer(const AST_Tree &tree);
    ~SemanticAnalyzer();

    void Analyze();
    const Table<Function_Data> &GetFunctionTable() const;
};


#endif //RUSTCOMPILER_SEMANTICANALYZER_H
