#ifndef RUSTCOMPILER_AST_TREE_H
#define RUSTCOMPILER_AST_TREE_H

#include "Node.h"
#include <forward_list>
#include <functional>

class AST_Tree {
private:
    Node* _root;

public:
    explicit AST_Tree(Node* const& node = nullptr);
    ~AST_Tree() = default;
    Node* const& GetRoot() const;
    void Traversal() const;

    template<typename... T>
    static void DeleteNode(T& ... nodes) {
        for (auto &node : std::forward_list<std::reference_wrapper<Node*>>{nodes...}) {
            if (node.get()) {
                delete node.get();
                node.get() = nullptr;
            }
        }
    }
};

#endif //RUSTCOMPILER_AST_TREE_H
