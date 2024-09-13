#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

    enum class TokenType
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
        PROGRAM,
        VAR,
        INTEGER,
        REAL,
        BOOLEAN,
        PROCEDURE,
        BEGIN,
        END,
        IF,
        THEN,
        ELSE,
        WHILE,
        DO,
        NOT,
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
        EOF_TOKEN
    };

class Token
{
private:
    TokenType type;
    std::string text;

public:
    Token(TokenType type = TokenType::NONE, const std::string &text = "");

    TokenType getType() const;
    void setType(TokenType newType);
    std::string getText() const;

    friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

#endif
