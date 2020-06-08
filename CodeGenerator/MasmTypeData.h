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
    std::pair<MASMType, std::string> type;
    bool isInitialize;
    float value;

    explicit MasmID_Data(const std::string &id = "", const std::string &uid = "",
            const std::pair<MASMType, std::string>& type = std::make_pair(MASMType::None, "None")) {
        this->id = id;
        this->uid = uid;
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
    std::pair<MASMType, std::string> type;
    bool isPtr;
    bool isInitialize;
    unsigned int elementCount;
    std::vector<float> values;

    explicit MasmArray_Data(const std::string &id = "", const std::string &uid = "",
            const std::pair<MASMType, std::string>& type = std::make_pair(MASMType::None, "None"),
            const unsigned int &count = 0, const bool &isPtr = false) : values(count) {
        this->id = id;
        this->uid = uid;
        this->type = type;
        this->isPtr = isPtr;
        isInitialize = false;
        elementCount = count;
    }

    bool operator == (const std::string &id) const {
        return this->id == id;
    }

    bool operator == (const MasmArray_Data &data) const {
        return this->id == data.id;
    }
};


#endif //RUSTCOMPILER_MASMTYPEDATA_H
