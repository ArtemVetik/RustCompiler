#ifndef RUSTCOMPILER_TOKENTYPE_H
#define RUSTCOMPILER_TOKENTYPE_H

enum TokenType {
    NUM,     // Переменная
    ID,      // Константа
    DLM,     // Разделитель
    KEY,     // Ключевое слово
    RNUM,    // Вещественное число
    WRITE,   // Writeln
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
