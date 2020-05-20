#ifndef RUSTCOMPILER_IDTABLE_H
#define RUSTCOMPILER_IDTABLE_H

#include <string>
#include <vector>
#include <algorithm>
#include "TypeData.h"

struct Data {
    std::string id;
    TypeData type;

    explicit Data() : id() { }

    bool operator == (const std::string &id) const {
        return this->id == id;
    }

    bool operator == (const Data &data) const {
        return this->id == data.id;
    }
};

struct ID_Data : public Data {
    bool isInitialized;
    bool isMutable;

    ID_Data() : isMutable(false), isInitialized(false) { }

    ID_Data(const bool &isInitialized, const bool &isMutable, const std::string &id, const TypeData &type){
        this->isInitialized = isInitialized;
        this->isMutable = isMutable;
        this->id = id;
        this->type = type;
    }
};

struct Array_Data : public Data {

};

struct Function_Data : public Data {
    std::vector<Data*> parameters;

    Function_Data() : parameters() { }
};

#endif //RUSTCOMPILER_IDTABLE_H
