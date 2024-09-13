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
    NONE
};

class Token
{
private:
    TokenType type;
    std::string text;
    std::string row;

public:
    Token(TokenType type = TokenType::NONE, const std::string &text = "", const std::string &row = "");

    TokenType getType() const;
    void setType(TokenType newType); // Removido o Token:: desnecessário
    std::string getRow() const;
    std::string getText() const;

    friend std::ostream &operator<<(std::ostream &os, const Token &token);
};

#endif
