#ifndef RUSTCOMPILER_NODE_H
#define RUSTCOMPILER_NODE_H

#include <string>
#include <iostream>
#include <vector>

struct NodeData {
    std::string value;

    explicit NodeData(const std::string &string) {
        value = string;
    }

    explicit NodeData(const NodeData &data) {
        value = data.value;
    }
};

class Node {
private:
    NodeData* _data;
    std::vector<Node*> _childs;
    Node* _parent;

public:
    explicit Node(NodeData* const &data);
    explicit Node(const Node &node);
    explicit Node();
    ~Node();
    void SetParent(Node* const &parent);
    void AddData(NodeData* const &data);
    Node* &GetParent();
    NodeData* GetData() const;
    void AddChild(Node *const &child);
    void Traversal();
};

#endif //RUSTCOMPILER_NODE_H
