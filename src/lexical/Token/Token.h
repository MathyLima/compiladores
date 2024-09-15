// Token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

enum TokenType
{
    IDENTIFIER,
    KEYWORD,
    NUMBER,
    FLOAT_NUMBER,
    ADD_OPERATOR,
    MULT_OPERATOR,
    REL_OPERATOR,
    EQUAL_OPERATOR,
    DELIMITER,
    LOGICAL_OPERATOR,
    ASSIGNMENT,
    LITERAL,
    REL_FUNCTION,
    NONE,
    PROGRAM = 101,
    VAR = 102,
    INTEGER = 103,
    REAL = 104,
    BOOLEAN = 105,
    PROCEDURE = 106,
    BEGIN = 107,
    END = 108,
    IF = 109,
    THEN = 110,
    ELSE = 111,
    WHILE = 112,
    DO = 113,
    NOT = 114,
    AND =115,
    OR = 116,
    COMMA,
    SEMICOLON,
    DOT,
    COLON,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    EOF_TOKEN,
    TRUE,
    FALSE,
    FOR,
    TO
};

// Classe para token
class Token
{
public:
    TokenType type;
    std::string value;
    std::string line;

    Token(TokenType type = NONE, const std::string &value = "", const std::string &line = "")
        : type(type), value(value), line(line) {}

    TokenType getType() const { return type; }
    void setType(TokenType newType) { type = newType; }
    std::string getRow() const { return line; }
    std::string getText() const { return value; }

    friend std::ostream &operator<<(std::ostream &os, const Token &token)
    {
        os << "Token(Type: " << token.type << ", Value: " << token.value << ", Line: " << token.line << ")";
        return os;
    }
};

#endif // TOKEN_H
