#include "Node.h"

Node::Node(NodeData *const &data) {
    _data = new NodeData(*data);
    _parent = nullptr;
}

void Node::AddChild(Node *const &child) {
    if (child)
        _childs.emplace_back(new Node(*child));
    else
        _childs.emplace_back(nullptr);
}

void Node::Traversal() {
    std::cout << "DATA: " << _data->token.GetValue() << std::endl;

    for (auto const& child : _childs) {
        if (child)
            child->Traversal();
    }
}

Node::Node() {
    _data = nullptr;
    _parent = nullptr;
}

void Node::AddData(NodeData *const &data) {
    _data = new NodeData(*data);
}

NodeData *Node::GetData() const{
    return _data;
}

void Node::SetParent(Node * const &parent) {
    _parent = new Node(*parent);
}

Node* &Node::GetParent() {
    return _parent;
}

Node::Node(const Node &node) {
    _parent = nullptr;
    _data = nullptr;

    if (node._data)
        _data = new NodeData(*(node._data));
    if (node._parent)
        _parent = new Node(*(node._parent));
    for (auto child : node._childs){
        AddChild(child);
    }
}

Node::~Node() {
    if (_data) {
        delete _data;
        _data = nullptr;
    }
    if (_parent) {
        delete _parent;
        _parent = nullptr;
    }
    for (Node* child : _childs){
        delete child;
        child = nullptr;
    }
    _childs.clear();
}
