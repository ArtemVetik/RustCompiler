#ifndef RUSTCOMPILER_NODE_H
#define RUSTCOMPILER_NODE_H

#include <string>
#include <iostream>
#include <vector>
#include "../../Lexer/Token.h"
#include "RuleType.h"

struct NodeData {
    Token token;
    RuleType ruleType;

    explicit NodeData() : token(), ruleType() { }

    explicit NodeData(const Token &token, const RuleType &type) {
        this->token = token;
        this->ruleType = type;
    }

    NodeData(const NodeData &data) {
        this->token = data.token;
        this->ruleType = data.ruleType;
    }
};

class Node {
private:
    NodeData* _data;
    std::vector<Node*> _childs;

public:
    explicit Node(NodeData* const &data);

    Node(const Node &node);
    explicit Node();
    ~Node();
    void AddChild(Node *const &child);
    const std::vector<Node*> &GetChilds() const;
    Node* const &GetChild(const unsigned int &ind) const;
    NodeData* GetData() const;
    void Traversal();
};

#endif //RUSTCOMPILER_NODE_H
