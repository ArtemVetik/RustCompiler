#include <utility>

#ifndef RUSTCOMPILER_NODE_H
#define RUSTCOMPILER_NODE_H

#include <string>
#include <iostream>
#include <vector>
#include "../Lexer/Token.h"
#include "../Semantics/RuleType.h"

struct NodeData {
    Token token;
    RuleType type;

    explicit NodeData() : token(), type() { }

    explicit NodeData(const Token &token, const RuleType &type) {
        this->token = token;
        this->type = type;
    }

    NodeData(const NodeData &data) {
        this->token = data.token;
        this->type = data.type;
    }
};

class Node {
private:
    NodeData* _data;
    std::vector<Node*> _childs;
    Node* _parent;

public:
    explicit Node(NodeData* const &data);

    Node(const Node &node);
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
