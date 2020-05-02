#ifndef RUSTCOMPILER_AST_TREE_H
#define RUSTCOMPILER_AST_TREE_H

#include "Node.h"

class AST_Tree {
private:
    Node* _root;

public:
    explicit AST_Tree(Node* const& node);
    ~AST_Tree() = default;
    Node* const& GetRoot() const;
    void Traversal() const;
};

#endif //RUSTCOMPILER_AST_TREE_H
