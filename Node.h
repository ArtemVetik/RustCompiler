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
};

class Node {
private:
    NodeData* _data;
    std::vector<Node*> _childs;

public:
    explicit Node(NodeData* const &data);
    explicit Node();
    void AddData(NodeData* const &data);
    NodeData* GetData() const;
    void AddChild(Node *const &node);
    void Traversal();
};

#endif //RUSTCOMPILER_NODE_H
