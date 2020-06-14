#ifndef RUSTCOMPILER_IDTABLE_H
#define RUSTCOMPILER_IDTABLE_H

#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include "TypeData.h"
#include "Table.h"

struct Data {
    TokenLocation location;
    std::string id;
    TypeData type;

    explicit Data() : id(), type(Type::None) { }

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
    bool isMutable;
    bool isInitialized;
    unsigned int elementCount;

    Array_Data() : isMutable(false), isInitialized(false), elementCount(0) { }
};

struct Function_Data : public Data {
    bool hasReturn;
    std::vector<Data*> parameters;

    Function_Data() : parameters(), hasReturn(false) { }
    Function_Data(const std::string &id, const TypeData &type, std::vector<Data*> &&parameters = {}) : parameters(parameters) {
        this->id = id;
        this->type = type;
        this->hasReturn = false;
    }
};

#endif //RUSTCOMPILER_IDTABLE_H
