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

Node::Node() {
    _data = nullptr;
    _parent = nullptr;
}

void Node::AddData(NodeData *const &data) {
    _data = data;
}

NodeData *Node::GetData() const{
    return _data;
}

void Node::SetParent(Node * &parent) {
    _parent = new Node(*parent->GetParent());
}

Node* &Node::GetParent() {
    return _parent;
}

Node::Node(const Node &node) {
    if (node._data)
        _data = new NodeData(*node._data);
    if (node._parent)
        _parent = new Node(*_parent);
    for (auto child : node._childs){
        _childs.emplace_back(new Node(*child));
    }
}