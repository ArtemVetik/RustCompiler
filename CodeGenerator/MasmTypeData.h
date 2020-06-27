#ifndef RUSTCOMPILER_MASMTYPEDATA_H
#define RUSTCOMPILER_MASMTYPEDATA_H

#include <string>
#include <utility>

/// @file MasmTypeData.h

/// @brief тип в макро ассемблере
enum class MASMType {
    DWORD, ///< целочисленный тип
    REAL8, ///< вещественный тип
    None, ///< обозначает отсутствие типа
};

/// @brief описывает переменную в макро ассемблере
struct MasmID_Data {
    std::string id; ///< @brief идентификатор
    std::string uid; ///< @brief уникальный идентификатор
    std::pair<MASMType, std::string> type; ///< @brief тип переменной в паре со строковым предтавлением типа
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

/// @brief описывает массив в макро ассемблере
struct MasmArray_Data {
    std::string id; ///< @brief идентификатор
    std::string uid; ///< @brief уникальный идентификатор
    std::pair<MASMType, std::string> type; ///< @brief тип переменной в паре со строковым предтавлением типа
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
