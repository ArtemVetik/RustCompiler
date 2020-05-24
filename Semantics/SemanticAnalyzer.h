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
    ProgramBlock *_currentBlock;

    void AddSystemFunctions();

    void Traversal(Node* const &root);
    void CheckRule(Node* const &node);

    void VariableDeclaration(Node* const &node);
    ID_Data Pat(Node* const &node);
    std::vector<ID_Data> GroupLetVarDeclaration(Node *const &node);
    std::vector<TypeData> GroupInit(std::vector<ID_Data> variables, Node* const &node);
    void ArrayDeclaration(Node* const &node);
    std::pair<TypeData, bool> Expr(Node* const &node);
    std::pair<TypeData, bool> BoolExpr(Node* const &node);
    std::pair<TypeData, bool> MinTerminal(Node* const &node);
    TypeData FunctionInvoke(Node* const &node);
    std::vector<std::pair<TypeData, bool>> FunctionInvokeParams(const std::string &funcId, Node* const &node);
    TypeData MemberExpr(Node* const &node);
    std::vector<TypeData> ArrayElems(Node* const &node);
    void Assignment(Node* const &node);
    void Condition(Node* const &node);
    void FunctionDeclaration(Node* const &node);
    void VariableAssignment(Node* const& idNode, Node *const &exprNode);
    void ArrayAssignment(Node* const& idNode, Node *const &exprNode);
    void MemberAssignment(Node* const& idNode, Node *const &exprNode);
    TypeData CanAccessIdentifier(Node* const &idNode);
    TypeData CanAccessArray(Node* const &idNode);
    bool HasIDInUpper(const std::string &id, Node *const &root);
    bool HasArrInUpper(const std::string &id, Node *const &root);
    ID_Data& GetID(const std::string &id, Node* const &root);
    Array_Data& GetArr(const std::string &id, Node* const &root);
    TypeData GetTypeData(Node* const &typeNode);
    ID_Data GetIDDefineParameter(Node* const &param);
    Array_Data GetArrayDefineParameter(Node* const &param);
    void ReturnExpression(Node* const &returnNode);

public:
    explicit SemanticAnalyzer(Node* const &root);
    ~SemanticAnalyzer();
    void Analyze();
};


#endif //RUSTCOMPILER_SEMANTICANALYZER_H
