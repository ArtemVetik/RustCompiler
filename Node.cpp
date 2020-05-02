#include "Node.h"

Node::Node(NodeData *const &data) {
    _data = data;
}

void Node::AddChild(Node *const &node) {
    _childs.emplace_back(node);
}
