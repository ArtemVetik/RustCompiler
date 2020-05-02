#include "AST_Tree.h"

AST_Tree::AST_Tree() {
    _root = new Node(new NodeData("Tree Root"));
}

Node* const& AST_Tree::GetRoot() const {
    return _root;
}
