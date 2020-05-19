#ifndef RUSTCOMPILER_TYPEDATA_H
#define RUSTCOMPILER_TYPEDATA_H

#include "../Lexer/TokenType.h"

enum class Type {
    None,

    Real,
    Integer,
    Unsigned
};

struct TypeData {
    bool isReference;
    bool isMutable;
    Type type;

    explicit TypeData() : isReference(false), isMutable(false), type(Type::None) { }

    TypeData(const bool &isReference, const bool &isMutable, const Type &type){
        this->isReference = isReference;
        this->isMutable = isMutable;
        this->type = type;
    }

    static Type ToType(const TokenType &tokenType){
        switch (tokenType){
            case REAL:
                return Type::Real;
            case INTEGER:
                return Type::Integer;
            case UINT:
                return Type::Unsigned;
            default:
                return Type::None;
        }
    }
};

#endif //RUSTCOMPILER_TYPEDATA_H
