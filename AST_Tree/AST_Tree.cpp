#include "AST_Tree.h"

AST_Tree::AST_Tree(Node* const& root) {
    _root = root;
}

Node* const& AST_Tree::GetRoot() const {
    return _root;
}

void AST_Tree::Traversal() const {
    _root->Traversal();
}