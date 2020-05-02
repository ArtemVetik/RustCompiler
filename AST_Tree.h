#ifndef RUSTCOMPILER_AST_TREE_H
#define RUSTCOMPILER_AST_TREE_H

#include "Node.h"

class AST_Tree {
private:
    Node* _root;

public:
    AST_Tree();
    ~AST_Tree() = default;
    Node* const& GetRoot() const;
};

#endif //RUSTCOMPILER_AST_TREE_H
