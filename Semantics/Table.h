#ifndef RUSTCOMPILER_TABLE_H
#define RUSTCOMPILER_TABLE_H

#include <algorithm>
#include <string>
#include <vector>
#include <exception>
#include "../Exceptions/SemanticError.h"

template <typename T>
class Table {
private:
    std::vector<T> _table;

public:
    void AddToTable(const T &data);
    bool Has(const std::string &value) const;
    const T& GetData(const std::string &id) const;
};

template <typename T>
void Table<T>::AddToTable(const T &data) {
    if (std::find(_table.cbegin(), _table.cend(), data) != _table.cend())
        throw 2;

    _table.emplace_back(data);
}

template<typename T>
bool Table<T>::Has(const std::string &value) const {
    return std::find(_table.cbegin(), _table.cend(), value) != _table.cend();
}

template<typename T>
const T &Table<T>::GetData(const std::string &id) const {
    auto data = std::find(_table.cbegin(), _table.cend(), id);
    if (data == _table.cend())
        throw "Table error:\n\tUnable to get data";
}

#endif //RUSTCOMPILER_TABLE_H
