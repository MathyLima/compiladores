#include "Parser.h"
#include <iostream>


// CODIGO PRECISANDO DE MUITOS AJUSTES AINDA

Parser::Parser(Scanner &scanner) : scanner(scanner)
{
   currentToken = scanner.getCurrentToken();
}

void Parser::parseProgram()
{
    std::cout << "Entering parseProgram" << std::endl;
    
    std::cout << "First token in parseProgram: " << currentToken.getText() << std::endl;

    // Consome o token "program"
    if (currentToken.getText() == "program")
    {
        match("program");
        match(TokenType::IDENTIFIER);
        // Consome o token ";"
        match(";");

        // Agora, prossegue para as outras partes do programa
        parseDeclarations();
        parseSubprogramDeclarations();
        parseCompoundCommand();

        // Verifica se o programa termina com "."
        match(".");
    }
    else
    {
        error("Expected 'program'");
    }
}

void Parser::match(const std::string &expected)
{
    std::cout << "Matching: |" << expected << "| with |" << currentToken.getText() << "|" << std::endl;

    if (currentToken.getText().compare(expected) == 0)
    {
        std::cout << "Token matched successfully!" << std::endl;
        currentToken = scanner.nextToken();
        std::cout << "Next Token Text: " << currentToken.getText() << std::endl;
    }
    else
    {
        error("Expected '" + expected + "'");
    }
}

void Parser::match(TokenType expectedType)
{
    std::cout << "Expected Token Type: " << static_cast<int>(expectedType) << std::endl;
    std::cout << "Current Token Type: " << static_cast<int>(currentToken.getType()) << std::endl;
    std::cout << "Current Token Text: " << currentToken.getText() << std::endl;

    if (currentToken.getType() == expectedType)
    {
        std::cout << "Token matched successfully!" << std::endl;
        currentToken = scanner.nextToken();
        std::cout << "Next Token Text: " << currentToken.getText() << std::endl;
        std::cout << "Next Token Type: " << static_cast<int>(currentToken.getType()) << std::endl;
    }
    else
    {
        error("Unexpected token type");
    }
}

std::string cleanString(const std::string &str)
{
    std::string result;
    for (char c : str)
    {
        if (!isspace(c))
        {
            result += c;
        }
    }
    return result;
}

// void Parser::match(const std::string &expected)
// {
//     std::cout << "Matching: |" << expected << "| with |" << currentToken.getText() << "|" << std::endl;

//     if (currentToken.getText().compare(expected) == 0)
//     {
//         std::cout << "Token matched successfully!" << std::endl;
//         currentToken = scanner.nextToken();
//         std::cout << "Next Token Text: " << currentToken.getText() << std::endl;
//     }
//     else
//     {
//         error("Expected '" + expected + "'");
//     }
// }

// void Parser::match(TokenType expectedType)
// {
//     std::cout << "Expected Token Type: " << static_cast<int>(expectedType) << std::endl;
//     std::cout << "Current Token Type: " << static_cast<int>(currentToken.getType()) << std::endl;
//     std::cout << "Current Token Text: " << currentToken.getText() << std::endl;

//     if (currentToken.getType() == expectedType)
//     {
//         std::cout << "Token matched successfully!" << std::endl;
//         currentToken = scanner.nextToken();
//         std::cout << "Next Token Text: " << currentToken.getText() << std::endl;
//         std::cout << "Next Token Type: " << static_cast<int>(currentToken.getType()) << std::endl;
//     }
//     else
//     {
//         error("Unexpected token type");
//     }
// }

void Parser::parseDeclarations()
{
    if (currentToken.getText() == "var")
    {
        match("var");
        parseVariableDeclarations();
    }
}

void Parser::parseVariableDeclarations()
{
    parseIdentifierList();
    match(":");
    parseType();
    match(";");
    if (currentToken.getType() == TokenType::IDENTIFIER)
    {
        parseVariableDeclarations();
    }
}

void Parser::parseIdentifierList()
{
    match(TokenType::IDENTIFIER);
    while (currentToken.getText() == ",")
    {
        match(",");
        match(TokenType::IDENTIFIER);
    }
}

void Parser::parseType()
{
    if (currentToken.getText() == "integer" || currentToken.getText() == "real" || currentToken.getText() == "boolean")
    {
        match(currentToken.getText());
    }
    else
    {
        error("Expected type");
    }
}

void Parser::parseSubprogramDeclarations()
{
    // Similar logic as parseDeclarations for handling subprograms
    if (currentToken.getText() == "procedure")
    {
        match("procedure");
        match(TokenType::IDENTIFIER);
        // Handle arguments and subprogram details here
        match(";");
        parseDeclarations();
        parseSubprogramDeclarations();
        parseCompoundCommand();
        match(";");
    }
}

void Parser::parseCompoundCommand()
{
    match("begin");
    parseOptionalCommands();
    match("end");
}

void Parser::parseOptionalCommands()
{
    if (currentToken.getType() == TokenType::IDENTIFIER ||
        currentToken.getText() == "if" ||
        currentToken.getText() == "while" ||
        currentToken.getText() == "begin")
    {
        parseCommandList();
    }
}

void Parser::parseCommandList()
{
    parseCommand();
    while (currentToken.getText() == ";")
    {
        match(";");
        parseCommand();
    }
}

void Parser::parseCommand()
{
    if (currentToken.getType() == TokenType::IDENTIFIER)
    {
        // Handle assignment or procedure activation
        match(TokenType::IDENTIFIER);
        if (currentToken.getText() == ":=")
        {
            match(":=");
            parseExpression();
        }
        else
        {
            // Handle procedure activation
            // (procedure might be called without parameters or with parameters)
        }
    }
    else if (currentToken.getText() == "if")
    {
        match("if");
        parseExpression();
        match("then");
        parseCommand();
        if (currentToken.getText() == "else")
        {
            match("else");
            parseCommand();
        }
    }
    else if (currentToken.getText() == "while")
    {
        match("while");
        parseExpression();
        match("do");
        parseCommand();
    }
    else if (currentToken.getText() == "begin")
    {
        parseCompoundCommand();
    }
    else
    {
        error("Unrecognized command");
    }
}

void Parser::parseExpression()
{
    parseSimpleExpression();
    if (currentToken.getType() == TokenType::REL_OPERATOR)
    {
        match(currentToken.getText());
        parseSimpleExpression();
    }
}

void Parser::parseSimpleExpression()
{
    parseTerm();
    while (currentToken.getType() == TokenType::ADD_OPERATOR)
    {
        match(currentToken.getText());
        parseTerm();
    }
}

void Parser::parseTerm()
{
    parseFactor();
    while (currentToken.getType() == TokenType::MULT_OPERATOR)
    {
        match(currentToken.getText());
        parseFactor();
    }
}

void Parser::parseFactor()
{
    if (currentToken.getType() == TokenType::IDENTIFIER)
    {
        match(TokenType::IDENTIFIER);
        if (currentToken.getText() == "(")
        {
            match("(");
            parseExpression();
            match(")");
        }
    }
    else if (currentToken.getType() == TokenType::NUMBER || currentToken.getType() == TokenType::FLOAT_NUMBER)
    {
        match(currentToken.getType());
    }
    else if (currentToken.getText() == "(")
    {
        match("(");
        parseExpression();
        match(")");
    }
    else if (currentToken.getText() == "not")
    {
        match("not");
        parseFactor();
    }
    else
    {
        error("Unrecognized factor");
    }
}

void Parser::error(const std::string &message)
{
    // std::cerr << "Error: " << message << " at row " << scanner.getRow() << ", col " << scanner.getCol() << std::endl;
    // exit(1);
    std::cerr << "Error: " << message << std::endl;
    exit(1);
}
