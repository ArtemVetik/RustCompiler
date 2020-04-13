#ifndef RUSTCOMPILER_TOKENTYPE_H
#define RUSTCOMPILER_TOKENTYPE_H
#include <string>

enum TokenType {
    ID,      // Константа
    INTNUM,  // Целое число
    RNUM,    // Вещественное число
    PRINTLN,  // Println
    STRING,  // Строка

    // Ключевые слова
    INTEGER,   // i32
    REAL,     // f32
    LET,     // let
    FOR,    // for
    IF,    // if
    ELSE, // else

    // Разделители
    PLUS,       // +
    MINUS,      // -
    MULT,       // *
    MOD,        // Вещественная часть
    DIV,        // Целая часть
    ASSIG,      // =

    // Операции отношения
    MORE,       // >
    LESS,       // <
    ASMR,       // >=
    ASLS,       // <=
    NASSIG,     // <>
    EQUAL,      // =
    EXCL,       // !

    LBLBR,      // {
    RBLBR,      // }
    SLBR,       // [
    SRBR,       // ]
    COLON,      // :
    SEMICOLON,  // ;
    LFBR,       // (
    RGBR,       // )
    COM,        // ,
    DOT,        // .
    IGNORE,     // Пробелы, табуляции, перенос строки
};

class TokenTypePrinter {
public:
    static std::string PrintTokenType(const TokenType &type) {
        switch (type) {
            case ID:return std::string("ID");
            case INTNUM:return std::string("INTNUM");
            case RNUM:return std::string("RNUM");
            case STRING:return std::string("STRING");
            case INTEGER:return std::string("INTEGER");
            case REAL:return std::string("REAL");
            case LET:return std::string("LET");
            case FOR:return std::string("FOR");
            case IF:return std::string("IF");
            case ELSE:return std::string("ELSE");
            case PLUS:return std::string("PLUS");
            case MINUS:return std::string("MINUS");
            case MULT:return std::string("MULT");
            case MOD:return std::string("MOD");
            case DIV:return std::string("DIV");
            case ASSIG:return std::string("ASSIG");
            case MORE:return std::string("MORE");
            case LESS:return std::string("LESS");
            case ASMR:return std::string("ASMR");
            case ASLS:return std::string("ASLS");
            case NASSIG:return std::string("NASSIG");
            case EQUAL:return std::string("EQUAL");
            case LBLBR:return std::string("LBLBR");
            case RBLBR:return std::string("RBLBR");
            case SLBR:return std::string("SLBR");
            case SRBR:return std::string("SRBR");
            case COLON:return std::string("COLON");
            case SEMICOLON:return std::string("SEMICOLON");
            case LFBR:return std::string("LFBR");
            case RGBR:return std::string("RGBR");
            case COM:return std::string("COM");
            case DOT:return std::string("DOT");
            case EXCL:return std::string("EXLC");
            case PRINTLN:return std::string("PRINTLN");
            case IGNORE:return std::string("IGNORE");
        }
        return std::string("");
    }
};

#endif //RUSTCOMPILER_TOKENTYPE_H