#include "Node.h"

Node::Node(NodeData *const &data) {
    _data = new NodeData(*data);
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
    _data = new NodeData();
}

NodeData *Node::GetData() const {
    return _data;
}

Node::Node(const Node &node) {
    _data = nullptr;

    if (node._data)
        _data = new NodeData(*(node._data));

    for (auto &child : node._childs) {
        AddChild(child);
    }
}

Node::~Node() {
    if (_data) {
        delete _data;
        _data = nullptr;
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

Node* const& Node::GetChild(const unsigned int &ind) const {
    return _childs.at(ind);
}
