#ifndef RUSTCOMPILER_AST_TREE_H
#define RUSTCOMPILER_AST_TREE_H

#include "Node.h"
#include <forward_list>
#include <functional>

/// @file AST_Tree.h

/// @brief Абстрактное синтаксическое дерево
class AST_Tree {
private:
    Node* _root;

public:
    explicit AST_Tree(Node* const& root = nullptr);
    ~AST_Tree() = default;
    /// @return корень дерева
    [[nodiscard]] Node* const& GetRoot() const;
    /// @brief Вывод дерева
    /// @note Осуществляет симметричный обход дерева и вывод данных в узлах
    void Print() const;

    /// @brief Удаление вершин
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
