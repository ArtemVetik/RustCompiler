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
    TypeData Pat(Node* const &node);
    void GroupLetVarDeclaration(Node *const &node);
    void GroupInit(Node* const &node);
    std::vector<TypeData> Expr(Node* const &node);
    TypeData BoolExpr(Node* const &node);
    TypeData MinTerminal(Node* const &node);
    TypeData FunctionInvoke(Node* const &node);
    TypeData MemberExpr(Node* const &node);
    std::vector<TypeData> ArrayElems(Node* const &node);

public:
    explicit SemanticAnalyzer(Node* const &root);
    void Analyze();
};


#endif //RUSTCOMPILER_SEMANTICANALYZER_H
