#ifndef RUSTCOMPILER_NODE_H
#define RUSTCOMPILER_NODE_H

#include <string>
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
    void AddChild(Node *const &node);
};

#endif //RUSTCOMPILER_NODE_H
