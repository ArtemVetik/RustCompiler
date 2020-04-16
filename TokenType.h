#ifndef RUSTCOMPILER_TOKENTYPE_H
#define RUSTCOMPILER_TOKENTYPE_H
#include <string>

enum TokenType {
    ID,      // Идентификатор
    INTNUM,  // Целое число
    RNUM,    // Вещественное число
    PRINTLN,  // Println
    STRING,  // Строка

    // Ключевые слова
    FUNCTION,   // fn
    INTEGER,    // i32
    REAL,       // f32
    UINT,       // usize
    LET,        // let
    MUT,        // mut
    FOR,        // for
    IF,         // if
    LOOP,       // loop
    WHILE,      // while
    ELSE,       // else
    BREAK,      // break
    AS,         // as

    // Разделители
    PLUS,       // +
    MINUS,      // -
    PLUSEQUAL,  // +=
    MINUSEQUAL, // -=
    MULT,       // *
    MOD,        // Вещественная часть
    DIV,        // Целая часть
    ASSIG,      // =
    EXCL,       // !
    LAND,       // &&
    LOR,        // ||


    // Операции отношения
    MORE,       // >
    LESS,       // <
    ASMR,       // >=
    ASLS,       // <=
    NASSIG,     // <>
    EQUAL,      // =

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
            case UINT:return std::string("UINT");
            case STRING:return std::string("STRING");
            case INTEGER:return std::string("INTEGER");
            case REAL:return std::string("REAL");
            case BREAK:return std::string("BREAK");
            case AS:return std::string("AS");
            case LET:return std::string("LET");
            case MUT:return std::string("MUT");
            case FOR:return std::string("FOR");
            case IF:return std::string("IF");
            case ELSE:return std::string("ELSE");
            case PLUS:return std::string("PLUS");
            case MINUS:return std::string("MINUS");
            case LOOP:return std::string("LOOP");
            case WHILE:return std::string("WHILE");
            case FUNCTION:return std::string("FUNCTION");
            case PLUSEQUAL:return std::string("PLUSEQUAL");
            case MINUSEQUAL:return std::string("MINUSEQUAL");
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