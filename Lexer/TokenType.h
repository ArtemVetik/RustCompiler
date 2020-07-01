#ifndef RUSTCOMPILER_TOKENTYPE_H
#define RUSTCOMPILER_TOKENTYPE_H
#include <string>

/// @file TokenType.h

/// @brief Тип покена
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
            case TokenType::ID:         return "ID";
            case TokenType::INTNUM:     return "INTNUM";
            case TokenType::RNUM:       return "RNUM";
            case TokenType::UINT:       return "UINT";
            case TokenType::STRINGLIT:  return "STRINGLIT";
            case TokenType::INTEGER:    return "INTEGER";
            case TokenType::REAL:       return "REAL";
            case TokenType::BREAK:      return "BREAK";
            case TokenType::LET:        return "LET";
            case TokenType::MUT:        return "MUT";
            case TokenType::IF:         return "IF";
            case TokenType::ELSE:       return "ELSE";
            case TokenType::PLUS:       return "PLUS";
            case TokenType::MINUS:      return "MINUS";
            case TokenType::LOOP:       return "LOOP";
            case TokenType::WHILE:      return "WHILE";
            case TokenType::FUNCTION:   return "FUNCTION";
            case TokenType::MULT:       return "MULT";
            case TokenType::MOD:        return "MOD";
            case TokenType::DIV:        return "DIV";
            case TokenType::ASSIG:      return "ASSIG";
            case TokenType::MORE:       return "MORE";
            case TokenType::LESS:       return "LESS";
            case TokenType::MOREEQUAL:  return "MOREEQUAL";
            case TokenType::LESSEQUAL:  return "LESSEQUAL";
            case TokenType::NASSIG:     return "NASSIG";
            case TokenType::EQUAL:      return "EQUAL";
            case TokenType::LFIGBR:     return "LFIGBR";
            case TokenType::RFIGBR:     return "RFIGBR";
            case TokenType::LSQRBR:     return "LSQRBR";
            case TokenType::RSQRBR:     return "RSQRBR";
            case TokenType::COLON:      return "COLON";
            case TokenType::SEMICOLON:  return "SEMICOLON";
            case TokenType::LRBR:       return "LRBR";
            case TokenType::RRBR:       return "RRBR";
            case TokenType::COM:        return "COM";
            case TokenType::DOT:        return "DOT";
            case TokenType::EXCL:       return "EXLC";
            case TokenType::PRINTLN:    return "PRINTLN";
            case TokenType::RETURN:     return "RETURN";
            case TokenType::IGNORE:     return "IGNORE";
            case TokenType::LAND:       return "LAND";
            case TokenType::LOR:        return "LOR";
            case TokenType::PRINT:      return "PRINT";
            case TokenType::BAND:       return "BAND";
            case TokenType::ARROW:      return "ARROW";
            case TokenType::BOOLLIT:    return "BOOLLIT";
            default:                    return "";
        }
    }
};

#endif //RUSTCOMPILER_TOKENTYPE_H