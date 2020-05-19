#ifndef RUSTCOMPILER_IDTABLE_H
#define RUSTCOMPILER_IDTABLE_H

#include <string>
#include <vector>
#include <algorithm>
#include "TypeData.h"

struct ID_Data {
    bool isMutable;
    std::string id;
    TypeData type;

    ID_Data() : isMutable(false), id(), type() { }

    ID_Data(const bool &isMutable, const std::string &id, const TypeData &type){
        this->isMutable = isMutable;
        this->id = id;
        this->type = type;
    }

    bool operator == (const ID_Data &data) const {
        return this->id == data.id;
    }

    bool operator == (const std::string &id) const {
        return this->id == id;
    }
};

struct Array_Data{
    std::string id;
    TypeData type;

    bool operator == (const std::string &id) const {
        return this->id == id;
    }
};

#endif //RUSTCOMPILER_IDTABLE_H
