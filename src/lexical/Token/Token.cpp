#include "Token.h"

Token::Token(TokenType type, const std::string &text, const std::string &row) : type(type), row(row) {}

TokenType Token::getType() const
{
    return type;
}

void Token::setType(TokenType newType)
{
    type = newType;
}

std::string Token::getText() const
{
    return text;
}

std::string Token::getRow() const
{
    return row;
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
    os << "Token: type: " << static_cast<int>(token.type) << ", text: " << token.text << "";
    return os;
}
