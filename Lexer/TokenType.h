#ifndef RUSTCOMPILER_TOKENTYPE_H
#define RUSTCOMPILER_TOKENTYPE_H
#include <string>

/// @file TokenType.h

/// @brief тип покена
enum class TokenType {
    ID,         ///< Идентификатор
    INTNUM,     ///< Целое число
    RNUM,       ///< Вещественное число
    STRINGLIT,  ///< Строка
    BOOLLIT,    ///< Логический тип

    // Ключевые слова
    PRINTLN,    ///< Println
    PRINT,      ///< Prints
    FUNCTION,   ///< fn
    INTEGER,    ///< i32
    REAL,       ///< f32
    UINT,       ///< usize
    BOOL,       ///< bool
    LET,        ///< let
    MUT,        ///< mut
    FOR,        ///< for
    IF,         ///< if
    LOOP,       ///< loop
    WHILE,      ///< while
    ELSE,       ///< else
    BREAK,      ///< break
    RETURN,     ///< return

    // Разделители
    PLUS,       ///< +
    MINUS,      ///< -
    MULT,       ///< *
    MOD,        ///< %
    DIV,        ///< /
    ASSIG,      ///< =
    EXCL,       ///< !
    LAND,       ///< &&
    BAND,       ///< &
    LOR,        ///< ||
    ARROW,      ///< ->

    // Операции отношения
    MORE,       ///< >
    LESS,       ///< <
    MOREEQUAL,  ///< >=
    LESSEQUAL,  ///< <=
    NASSIG,     ///< !=
    EQUAL,      ///< ==

    LFIGBR,     ///< {
    RFIGBR,     ///< }
    LSQRBR,     ///< [
    RSQRBR,     ///< ]
    COLON,      ///< :
    SEMICOLON,  ///< ;
    LRBR,       ///< (
    RRBR,       ///< )
    COM,        ///< ,
    DOT,        ///< .
    IGNORE,     ///< Пробелы, табуляции, перенос строки, комментарии
};

class TokenTypePrinter {
public:
    /// @return возвращает тип токена в виде строки
    static std::string PrintTokenType(const TokenType &type) {
        switch (type) {
            case TokenType::ID:return std::string("ID");
            case TokenType::INTNUM:return std::string("INTNUM");
            case TokenType::RNUM:return std::string("RNUM");
            case TokenType::UINT:return std::string("UINT");
            case TokenType::STRINGLIT:return std::string("STRINGLIT");
            case TokenType::INTEGER:return std::string("INTEGER");
            case TokenType::REAL:return std::string("REAL");
            case TokenType::BREAK:return std::string("BREAK");
            case TokenType::LET:return std::string("LET");
            case TokenType::MUT:return std::string("MUT");
            case TokenType::FOR:return std::string("FOR");
            case TokenType::IF:return std::string("IF");
            case TokenType::ELSE:return std::string("ELSE");
            case TokenType::PLUS:return std::string("PLUS");
            case TokenType::MINUS:return std::string("MINUS");
            case TokenType::LOOP:return std::string("LOOP");
            case TokenType::WHILE:return std::string("WHILE");
            case TokenType::FUNCTION:return std::string("FUNCTION");
            case TokenType::MULT:return std::string("MULT");
            case TokenType::MOD:return std::string("MOD");
            case TokenType::DIV:return std::string("DIV");
            case TokenType::ASSIG:return std::string("ASSIG");
            case TokenType::MORE:return std::string("MORE");
            case TokenType::LESS:return std::string("LESS");
            case TokenType::MOREEQUAL:return std::string("MOREEQUAL");
            case TokenType::LESSEQUAL:return std::string("LESSEQUAL");
            case TokenType::NASSIG:return std::string("NASSIG");
            case TokenType::EQUAL:return std::string("EQUAL");
            case TokenType::LFIGBR:return std::string("LFIGBR");
            case TokenType::RFIGBR:return std::string("RFIGBR");
            case TokenType::LSQRBR:return std::string("LSQRBR");
            case TokenType::RSQRBR:return std::string("RSQRBR");
            case TokenType::COLON:return std::string("COLON");
            case TokenType::SEMICOLON:return std::string("SEMICOLON");
            case TokenType::LRBR:return std::string("LRBR");
            case TokenType::RRBR:return std::string("RRBR");
            case TokenType::COM:return std::string("COM");
            case TokenType::DOT:return std::string("DOT");
            case TokenType::EXCL:return std::string("EXLC");
            case TokenType::PRINTLN:return std::string("PRINTLN");
            case TokenType::RETURN:return std::string("RETURN");
            case TokenType::IGNORE:return std::string("IGNORE");
            case TokenType::LAND:return std::string("LAND");
            case TokenType::LOR:return std::string("LOR");
            case TokenType::PRINT: return std::string("PRINT");
            case TokenType::BAND: return std::string("BAND");
            case TokenType::ARROW: return std::string("ARROW");
            case TokenType::BOOLLIT: return std::string("BOOLLIT");
            default: return "";
        }
    }
};

#endif //RUSTCOMPILER_TOKENTYPE_H