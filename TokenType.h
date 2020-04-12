#ifndef RUSTCOMPILER_TOKENTYPE_H
#define RUSTCOMPILER_TOKENTYPE_H

enum TokenType {
    NUM,     // Переменная
    ID,      // Константа
    DLM,     // Разделитель
    KEY,     // Ключевое слово
    INTNUM,  //
    RNUM,    // Вещественное число
    PRINTLN,   // Println
    READ,    // Readln
    STRING,  // Стока

    // Ключевые слова
    INTEGER,
    REAL,
    VAR,
    CONST,
    WHILE,
    DO,
    THEN,
    LET,
    FOR,
    IF,
    ELSE,
    OR,
    AND,
    NOT,
    BEGIN,
    END,
    TYPE,
    OF,
    ARRAY,
    FUNC,

    // Разделители
    PLUS,       // +
    MINUS,      // -
    MULT,       // *
    SL,         // /
    MOD,        // Вещественная
    DIV,        // Целая часть

    ASSIG,      // :=

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
    QUOTE,      // "

    IGNORE,
};

#endif //RUSTCOMPILER_TOKENTYPE_H