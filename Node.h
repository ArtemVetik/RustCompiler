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

    void Debug(){
        std::cout << _data->value << " " << _parent->GetData()->value << " " << _childs[0]->GetData()->value << " " << _childs[1]->GetData()->value << std::endl;
    }

    explicit Node(NodeData* const &data);
    explicit Node(const Node &node);
    explicit Node();
    void SetParent(Node* &parent);
    void AddData(NodeData* const &data);
    Node* &GetParent();
    NodeData* GetData() const;
    void AddChild(Node *const &node);
    void Traversal();
};

#endif //RUSTCOMPILER_NODE_H
