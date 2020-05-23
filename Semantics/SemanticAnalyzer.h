#ifndef RUSTCOMPILER_SEMANTICANALYZER_H
#define RUSTCOMPILER_SEMANTICANALYZER_H

#include "../AST_Tree/AST_Tree.h"
#include "../Exceptions/SemanticError.h"
#include "TableData.h"
#include "Table.h"

class SemanticAnalyzer {
private:
    Node* _root;
    Table<ID_Data> _idTable;
    Table<Array_Data> _arrayTable;
    Table<Function_Data> _functionTable;

    void Traversal(Node* const &root);
    void CheckRule(Node* const &node);

    void VariableDeclaration(Node* const &node);
    ID_Data Pat(Node* const &node);
    std::vector<ID_Data> GroupLetVarDeclaration(Node *const &node);
    std::vector<TypeData> GroupInit(std::vector<ID_Data> variables, Node* const &node);
    void ArrayDeclaration(Node* const &node);
    TypeData Expr(Node* const &node);
    TypeData BoolExpr(Node* const &node);
    TypeData MinTerminal(Node* const &node);
    TypeData FunctionInvoke(Node* const &node);
    std::vector<TypeData> FunctionParams(std::string funcId, Node* const &node);
    TypeData MemberExpr(Node* const &node);
    std::vector<TypeData> ArrayElems(Node* const &node);
    void Assignment(Node* const &node);

    void VariableAssignment(std::string id, Node *const &exprNode);
    void ArrayAssignment(std::string id, Node *const &exprNode);
    void MemberAssignment(std::string id, Node *const &exprNode);
    TypeData CanAccessIdentifier(Node* const &idNode);
    TypeData CanAccessArray(Node* const &idNode);

public:
    explicit SemanticAnalyzer(Node* const &root);
    void Analyze();
};


#endif //RUSTCOMPILER_SEMANTICANALYZER_H
