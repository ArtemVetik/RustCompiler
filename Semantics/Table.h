#ifndef RUSTCOMPILER_TABLE_H
#define RUSTCOMPILER_TABLE_H

#include <algorithm>
#include <string>
#include <vector>
#include <exception>

template <typename T>
class Table {
private:
    std::vector<T> _table;

public:
    void AddToTable(const T &data);
    bool Has(const std::string &value) const;
    T& GetData(const std::string &id);
    const std::vector<T> &GetTable() const;
    T Back();
};

template <typename T>
void Table<T>::AddToTable(const T &data) {
    _table.emplace_back(data);
}

template<typename T>
bool Table<T>::Has(const std::string &value) const {
    return std::find(_table.cbegin(), _table.cend(), value) != _table.cend();
}

template<typename T>
T &Table<T>::GetData(const std::string &id) {
    auto data = std::find(_table.cbegin(), _table.cend(), id);
    if (data == _table.cend())
        throw "Table _error:\n\tUnable to get data";
    return _table[std::distance(_table.cbegin(), data)];
}

template<typename T>
const std::vector<T> &Table<T>::GetTable() const {
    return _table;
}

template<typename T>
T Table<T>::Back() {
    return _table.back();
}

#endif //RUSTCOMPILER_TABLE_H
