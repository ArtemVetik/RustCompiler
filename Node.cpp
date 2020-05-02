#include "Node.h"

Node::Node(NodeData *const &data) {
    _data = data;
}

void Node::AddChild(Node *const &node) {
    _childs.emplace_back(node);
}

void Node::Traversal() {
    std::cout << "DATA: " << _data->value << std::endl;

    for (auto const& child : _childs) {
        child->Traversal();
    }
}
