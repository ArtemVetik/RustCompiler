#ifndef RUSTCOMPILER_SEMANTICANALYZER_H
#define RUSTCOMPILER_SEMANTICANALYZER_H

#include "../AST_Tree/AST_Tree.h"
#include "../Exceptions/SemanticError.h"
#include "TableData.h"
#include "Table.h"

class SemanticAnalyzer {
private:
    Table<ID_Data> _idTable;
    Table<Array_Data> _arrayTable;

    void Traversal(Node* const &root);
    void CheckRule(Node* const &node);

    void VariableDeclaration(Node* const &node);
    Type Pat(Node* const &node);
    void GroupLetVarDeclaration(Node *const &node);
    void GroupInit(Node* const &node);
    const std::vector<Type>& Expr(Node* const &node);
    Type BoolExpr(Node* const &node);
    const std::vector<Type>& ArrayExpr(Node* const &node);
    Type MemberExpr(Node* const &node);
    void ArrayElems(Node* const &node);

public:
    void Analyze(Node* const &root);
};


#endif //RUSTCOMPILER_SEMANTICANALYZER_H
