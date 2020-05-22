#ifndef RUSTCOMPILER_TYPEDATA_H
#define RUSTCOMPILER_TYPEDATA_H

#include "../Lexer/TokenType.h"

enum class Type {
    None,

    Bool,
    String,

    Real,
    Integer,
    Unsigned

};

struct TypeData {
    bool isReference;
    bool isMutable;
    Type type;

    explicit TypeData() : isReference(false), isMutable(false), type(Type::None) { }
    explicit TypeData(const Type &type) : isReference(false), isMutable(false), type(type) { }

    explicit TypeData(const bool &isReference , const bool &isMutable, const Type &type) {
        this->isReference = isReference;
        this->isMutable = isMutable;
        this->type = type;
    }

    bool operator == (const TypeData &right) const {
        return isReference == right.isReference &&
                isMutable == right.isMutable &&
                type == right.type;
    }

    bool operator != (const TypeData &right) const {
        return !(*this == right);
    }

    static Type ToType(const TokenType &tokenType) {
        switch (tokenType){
            case REAL:
            case RNUM:
                return Type::Real;
            case INTEGER:
            case INTNUM:
                return Type::Integer;
            case UINT:
                return Type::Unsigned;
            case STRING:
                return Type::String;
            case BOOL:
                return Type::Bool;
            default:
                return Type::None;
        }
    }

    std::string ToString() {
        std::string res;
        if (isReference) res += "& ";
        if (isMutable) res += "mut ";
        res += TypeData::ToString(type);
        return res;
    }

    static std::string ToString(const Type &type) {
        switch (type){
            case Type::None:
                return "None";
            case Type::Bool:
                return "Bool";
            case Type::Real:
                return "Real";
            case Type::Integer:
                return "Integer";
            case Type::Unsigned:
                return "Unsigned";
            case Type::String:
                return "String";
            default:
                return "";
        }
    }
};

#endif //RUSTCOMPILER_TYPEDATA_H
