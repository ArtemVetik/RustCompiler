#ifndef RUSTCOMPILER_MASMTYPEDATA_H
#define RUSTCOMPILER_MASMTYPEDATA_H

#include <string>
#include <utility>

enum class MASMType {
    DWORD, REAL8, None
};

struct MasmID_Data {
    std::string id;
    std::string uid;
    MASMType type;
    bool isInitialize;
    float value;

    explicit MasmID_Data(std::string &id, std::string uid, const MASMType &type) {
        this->id = id;
        this->uid = std::move(uid);
        this->type = type;
        isInitialize = false;
        value = 0;
    }


    bool operator == (const std::string &id) const {
        return this->id == id;
    }

    bool operator == (const MasmID_Data &data) const {
        return this->id == data.id;
    }
};

struct MasmArray_Data {
    std::string id;
    std::string uid;
    MASMType type;
    bool isInitialize;
    unsigned int elementCount;
    float *value;

    explicit MasmArray_Data(std::string &id, std::string uid, const MASMType &type) {
        this->id = id;
        this->uid = std::move(uid);
        this->type = type;
        isInitialize = false;
        elementCount = 0;
        value = nullptr;
    }

    bool operator == (const std::string &id) const {
        return this->id == id;
    }

    bool operator == (const MasmArray_Data &data) const {
        return this->id == data.id;
    }
};


#endif //RUSTCOMPILER_MASMTYPEDATA_H
