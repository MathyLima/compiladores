#include "Token.h"

Token::Token(TokenType type, const std::string &text) : type(type), text(text) {}

TokenType Token::getType() const
{
    return type;
}

std::string Token::getText() const
{
    return text;
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
    os << "Token: type: " << static_cast<int>(token.type) << ", text: " << token.text << "";
    return os;
}
