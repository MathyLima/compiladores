#ifndef PARSER_H
#define PARSER_H

#include "../lexical/Scanner/Scanner.h"

class Parser
{
public:
    Parser(Scanner &scanner);
    void parseProgram();

private:
    Scanner &scanner;
    Token currentToken;

    void match(const std::string &expected);
    void match(TokenType expectedType);
    void parseDeclarations();
    void parseVariableDeclarations();
    void parseIdentifierList();
    void parseType();
    void parseSubprogramDeclarations();
    void parseCompoundCommand();
    void parseOptionalCommands();
    void parseCommandList();
    void parseCommand();
    void parseExpression();
    void parseSimpleExpression();
    void parseTerm();
    void parseFactor();
    void error(const std::string &message);
};

#endif
