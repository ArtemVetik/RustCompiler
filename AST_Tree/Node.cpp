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
    static unsigned int deep = 0;
    std::cout << std::string(deep, '-') << _data->token.GetValue() << std::endl;

    deep += 4;
    for (auto const& child : _childs) {
        if (child)
            child->Traversal();
    }

    deep -= 4;
}

Node::Node() {
    _data = nullptr;
    _parent = nullptr;
}

NodeData *Node::GetData() const {
    return _data;
}

Node::Node(const Node &node) {
    _parent = nullptr;
    _data = nullptr;

    if (node._data)
        _data = new NodeData(*(node._data));
    if (node._parent)
        _parent = new Node(*(node._parent));
    for (auto child : node._childs) {
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

const std::vector<Node *> &Node::GetChilds() const {
    return _childs;
}

Node* const& Node::GetChild(unsigned int const &ind) const {
    return _childs.at(ind);
}
