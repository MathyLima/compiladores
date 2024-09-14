#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stack>
#include <unordered_set>
#include <functional>
#include "Parser.h"
#include "../lexical/Token/Token.h"

void print_ast(ASTNode *node, std::stack<std::string> &nodeStack, int indent)
{
    if (!node)
        return;

    // Imprime informações do nó raiz
    if (indent == 0)
    {
        std::cout << "Sou (Type: " << node->token.type << ", Value: " << node->token.value << ", Line: " << node->token.line << ") e tenho "
                  << node->children.size() << " filhos:" << std::endl;
        nodeStack.push(node->token.value); // Empilha o valor da raiz
    }

    // Itera sobre os filhos do nó
    for (ASTNode *child : node->children)
    {
        if (child == nullptr)
        {
            std::cerr << "Opa, child eh null, devo subir o nivel na arvore..." << std::endl;
            continue;
        }

        // Imprime indentação para indicar a profundidade do nó na árvore
        for (int i = 0; i < indent; ++i)
        {
            std::cout << "|";
        }

        // Imprime informações do nó filho
        std::cout << " Sou (Type: " << child->token.type << ", Value: " << child->token.value << ", Line: " << child->token.line << ") e sou filho do citado acima" << std::endl;
        nodeStack.push(child->token.value); // Empilha o valor do filho

        // Recursivamente imprime os filhos deste nó
        print_ast(child, nodeStack, indent + 1);
    }
}

void build_node_levels(ASTNode *node, NodeLevel &nodeLevels, int indent)
{
    if (!node)
        return;

    std::vector<std::string> currentLevel;

    // Verifica se o nó é um não-terminal
    bool isNonTerminal = node->token.type == NONE;

    // Adiciona o nó atual (pai) como o primeiro elemento do nível somente se for um não-terminal
    if (isNonTerminal)
    {
        currentLevel.push_back(
            "Type: " + std::to_string(node->token.type) +
            ", Value: " + node->token.value +
            ", Line: " + node->token.line);
    }

    // Adiciona os filhos ao mesmo nível
    for (ASTNode *child : node->children)
    {
        if (child == nullptr)
        {
            continue;
        }
        currentLevel.push_back(
            "Type: " + std::to_string(child->token.type) +
            ", Value: " + child->token.value +
            ", Line: " + child->token.line);
    }

    // Adiciona o nível atual ao NodeLevel
    if (!currentLevel.empty())
    {
        nodeLevels.addLevel(currentLevel);
    }

    // Agora, percorre os filhos recursivamente
    for (ASTNode *child : node->children)
    {
        build_node_levels(child, nodeLevels, indent + 1);
    }
}

// Implementação do construtor
Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current_token_index(0)
{
    current_token = tokens[current_token_index];
    convert_to_sintax_type();
}

// Implementação de parse_program
ASTNode *Parser::parse_program()
{
    ASTNode *root = new ASTNode({NONE, "Program", ""});

    root->addChild(expect(PROGRAM));
    root->addChild(expect(IDENTIFIER));
    root->addChild(expect(SEMICOLON));
    root->addChild(parse_block());
    root->addChild(expect(DOT));
    return root;
}

// Implementação do método advance
void Parser::advance()
{
    if (current_token_index < tokens.size() - 1)
    {
        current_token_index++;
        current_token = tokens[current_token_index];
        convert_to_sintax_type(); // Converter para o tipo correto a cada avanço
    }
}

// Implementação do método peek_next_token
Token Parser::peek_next_token()
{
    if (current_token_index < tokens.size() - 2)
    {
        Token next_token = tokens[current_token_index + 1];
        if (next_token.type == KEYWORD || next_token.type == DELIMITER || next_token.type == ADD_OPERATOR || next_token.type == MULT_OPERATOR)
        {
            next_token.type = reserved_words_to_token(current_token.value);
        }
        return next_token;
    }

    return current_token;
}

// Implementação do método convert_to_sintax_type
void Parser::convert_to_sintax_type()
{
    if (current_token.type == KEYWORD || current_token.type == DELIMITER ||
        current_token.type == ADD_OPERATOR || current_token.type == MULT_OPERATOR)
    {
        TokenType new_type = reserved_words_to_token(current_token.value);
        if (new_type != NONE)
        {
            current_token.type = new_type;
        }
    }
}

// Implementação do método expect
ASTNode *Parser::expect(TokenType expected_type)
{
    if (current_token.type == expected_type)
    {
        ASTNode *node = new ASTNode(current_token); // Use the current token as is
        advance();
        return node;
    }
    else
    {
        throw SyntaxError("Linha: " + current_token.line +
                          " Erro de sintaxe: " +
                          "Esperado: " + token_type_to_string(expected_type) + " mas encontrado " + current_token.value);
    }
}

// Implementação de is_command_start
bool Parser::is_command_start(TokenType type)
{
    return type == IDENTIFIER || type == BEGIN || type == IF || type == WHILE;
}

// Implementação de parse_block
ASTNode *Parser::parse_block()
{
    std::cout << "Estou dentro de parse_block" << std::endl;
    ASTNode *blockNode = new ASTNode({NONE, "Block", ""});
    blockNode->addChild(parse_var_declaration());
    blockNode->addChild(parse_declare_subprograms());
    blockNode->addChild(parse_compound_statement());
    return blockNode;
}

// Implementação de parse_var_declaration
ASTNode *Parser::parse_var_declaration()
{
    std::cout << "Estou dentro de parse_var_declaration" << std::endl;
    ASTNode *varDeclNode = new ASTNode({NONE, "VarDeclaration", ""});
    if (current_token.type == VAR)
    {
        varDeclNode->addChild(expect(VAR)); // Adiciona 'var' à AST
        varDeclNode->addChild(parse_var_list_declarations());
        // Continuação de varList caso tenha mais identificadores
        while (current_token.type == IDENTIFIER)
        {
            varDeclNode->addChild(parse_var_list_declarations());
        }
    }
    return varDeclNode;
}

// Implementação de parse_var_list_declarations
ASTNode *Parser::parse_var_list_declarations()
{
    std::cout << "Estou dentro de parse_var_list_declarations" << std::endl;
    ASTNode *varListNode = new ASTNode({NONE, "VarList", ""});

    do
    {
        varListNode->addChild(parse_identifier_list());
        varListNode->addChild(expect(COLON));
        varListNode->addChild(parse_type());
        varListNode->addChild(expect(SEMICOLON));
    } while (current_token.type == IDENTIFIER);

    return varListNode;
}

// Implementação de parse_identifier_list
ASTNode *Parser::parse_identifier_list()
{
    std::cout << "Estou dentro de parse_identifier_list" << std::endl;
    ASTNode *idListNode = new ASTNode({NONE, "IdentifierList", ""});
    idListNode->addChild(expect(IDENTIFIER));
    while (current_token.type == COMMA)
    {
        advance();
        idListNode->addChild(expect(IDENTIFIER));
    }
    return idListNode;
}

// Implementação de parse_type
ASTNode *Parser::parse_type()
{
    std::cout << "Estou dentro de parse_type" << std::endl;
    ASTNode *typeNode = nullptr;
    if (current_token.type == INTEGER)
    {
        typeNode = new ASTNode({INTEGER, "integer", current_token.line});
        advance();
    }
    else if (current_token.type == REAL)
    {
        typeNode = new ASTNode({REAL, "real", current_token.line});
        advance();
    }
    else if (current_token.type == BOOLEAN)
    {
        typeNode = new ASTNode({BOOLEAN, "boolean", current_token.line});
        advance();
    }
    else
    {
        throw SyntaxError("Erro de sintaxe: tipo de variável inválido");
    }
    return typeNode;
}

// Implementação de parse_declare_subprograms
ASTNode *Parser::parse_declare_subprograms()
{
    std::cout << "Estou dentro de parse_declare_subprograms" << std::endl;
    ASTNode *subprogramsNode = new ASTNode({NONE, "SubprogramDeclarations", ""});
    while (current_token.type == PROCEDURE)
    {
        subprogramsNode->addChild(parse_subprogram());
        subprogramsNode->addChild(expect(SEMICOLON));
    }
    return subprogramsNode;
}

// Implementação de parse_subprogram
ASTNode *Parser::parse_subprogram()
{
    std::cout << "Estou dentro de parse_subprogram" << std::endl;
    ASTNode *subprogramNode = new ASTNode({NONE, "ProcedureDeclaration", ""});
    subprogramNode->addChild(expect(PROCEDURE));
    subprogramNode->addChild(expect(IDENTIFIER));
    subprogramNode->addChild(parse_arguments());
    subprogramNode->addChild(expect(SEMICOLON));
    subprogramNode->addChild(parse_var_declaration());
    subprogramNode->addChild(parse_declare_subprograms());
    subprogramNode->addChild(parse_compound_statement());
    return subprogramNode;
}

// Implementação de parse_compound_statement
ASTNode *Parser::parse_compound_statement()
{
    std::cout << "Estou dentro de parse_compound_statement" << std::endl;
    ASTNode *compoundNode = new ASTNode({NONE, "CompoundStatement", ""});
    compoundNode->addChild(expect(BEGIN));
    compoundNode->addChild(parse_optional_commands());
    compoundNode->addChild(expect(END));
    return compoundNode;
}

// Implementação de parse_optional_commands
ASTNode *Parser::parse_optional_commands()
{
    std::cout << "Estou dentro de parse_optional_commands" << std::endl;
    if (current_token.type == END)
    {
        return new ASTNode({NONE, "Empty", ""});
    }
    return parse_command_list();
}

// Implementação de parse_command_list
ASTNode *Parser::parse_command_list()
{
    std::cout << "Estou dentro de parse_command_list" << std::endl;
    ASTNode *cmdListNode = new ASTNode({NONE, "CommandList", ""});
    cmdListNode->addChild(parse_command());
    while (current_token.type == SEMICOLON)
    {
        cmdListNode->addChild(expect(SEMICOLON)); // Adiciona o ';' à AST
        // Verifica se o próximo token pode iniciar um comando
        if (!is_command_start(current_token.type))
        {
            // Não há mais comandos, sai do loop
            break;
        }
        cmdListNode->addChild(parse_command());
    }
    return cmdListNode;
}

// Implementação de parse_command
ASTNode *Parser::parse_command()
{
    std::cout << "Estou dentro de parse_command" << std::endl;
    if (current_token.type == IDENTIFIER)
    {
        if (peek_next_token().type == ASSIGNMENT)
        {
            return parse_assignment_statement();
        }
        else
        {
            return parse_procedure_activation();
        }
    }
    else if (current_token.type == BEGIN)
    {
        return parse_compound_statement();
    }
    else if (current_token.type == IF)
    {
        return parse_if_statement();
    }
    else if (current_token.type == WHILE)
    {
        return parse_while_statement();
    }
    else
    {
        throw SyntaxError("Linha: " + current_token.line +
                          " Erro de sintaxe: comando inválido");
    }
}

// Implementação de parse_procedure_activation
ASTNode *Parser::parse_procedure_activation()
{
    std::cout << "Estou dentro de parse_procedure_activation" << std::endl;
    ASTNode *procActivationNode = new ASTNode({NONE, "ProcedureActivation", ""});
    procActivationNode->addChild(expect(IDENTIFIER));

    if (current_token.type == LPAREN)
    {
        advance();
        procActivationNode->addChild(parse_expression_list());
        expect(RPAREN);
    }
    return procActivationNode;
}

// Implementação de parse_expression_list
ASTNode *Parser::parse_expression_list()
{
    std::cout << "Estou dentro de parse_expression_list" << std::endl;
    ASTNode *exprListNode = new ASTNode({NONE, "ExpressionList", ""});
    exprListNode->addChild(parse_expression());
    while (current_token.type == COMMA)
    {
        advance();
        exprListNode->addChild(parse_expression());
    }
    return exprListNode;
}

// Implementação de parse_assignment_statement
ASTNode *Parser::parse_assignment_statement()
{
    std::cout << "Estou dentro de parse_assignment_statement" << std::endl;
    ASTNode *assignNode = new ASTNode({NONE, "AssignmentStatement", ""});
    assignNode->addChild(expect(IDENTIFIER));
    assignNode->addChild(expect(ASSIGNMENT));
    assignNode->addChild(parse_expression());
    return assignNode;
}

// Implementação de parse_if_statement
ASTNode *Parser::parse_if_statement()
{
    std::cout << "Estou dentro de parse_if_statement" << std::endl;
    ASTNode *ifNode = new ASTNode({NONE, "IfStatement", ""});
    ifNode->addChild(expect(IF));
    ifNode->addChild(parse_expression());
    ifNode->addChild(expect(THEN));
    ifNode->addChild(parse_command());
    ifNode->addChild(parse_else_part());
    return ifNode;
}

// Implementação de parse_else_part
ASTNode *Parser::parse_else_part()
{
    std::cout << "Estou dentro de parse_else_part" << std::endl;
    if (current_token.type == ELSE)
    {
        return expect(ELSE);
    }
    return new ASTNode({NONE, "NoElse", ""});
}

// Implementação de parse_while_statement
ASTNode *Parser::parse_while_statement()
{
    std::cout << "Estou dentro de parse_while_statement" << std::endl;
    ASTNode *whileNode = new ASTNode({NONE, "WhileStatement", ""});
    whileNode->addChild(expect(WHILE));
    whileNode->addChild(parse_expression());
    whileNode->addChild(expect(DO));
    whileNode->addChild(parse_command());
    return whileNode;
}

// Implementação de parse_expression
ASTNode *Parser::parse_expression()
{
    std::cout << "Estou dentro de parse_expression" << std::endl;
    ASTNode *exprNode = new ASTNode({NONE, "Expression", ""});
    exprNode->addChild(parse_simple_expression());
    if (current_token.type == REL_OPERATOR)
    {
        ASTNode *relOpNode = new ASTNode({REL_OPERATOR, current_token.value, current_token.line});
        advance();
        relOpNode->addChild(parse_simple_expression());
        exprNode->addChild(relOpNode);
    }
    return exprNode;
}

// Implementação de parse_simple_expression
ASTNode *Parser::parse_simple_expression()
{
    std::cout << "Estou dentro de parse_simple_expression" << std::endl;
    ASTNode *simpleExprNode = new ASTNode({NONE, "SimpleExpression", ""});
    if (current_token.type == PLUS || current_token.type == MINUS)
    {
        ASTNode *sinalNode = new ASTNode({current_token.type, current_token.value, current_token.line});
        advance();
        sinalNode->addChild(parse_term());
        simpleExprNode->addChild(sinalNode);
    }
    else
    {
        simpleExprNode->addChild(parse_term());
    }
    while (current_token.type == ADD_OPERATOR)
    {
        ASTNode *addOpNode = new ASTNode({ADD_OPERATOR, current_token.value, current_token.line});
        advance();
        addOpNode->addChild(parse_term());
        simpleExprNode->addChild(addOpNode);
    }
    return simpleExprNode;
}

// Implementação de parse_term
ASTNode *Parser::parse_term()
{
    std::cout << "Estou dentro de parse_term" << std::endl;
    ASTNode *termNode = new ASTNode({NONE, "Term", ""});
    termNode->addChild(parse_factor());

    while (current_token.type == MULTIPLY || current_token.type == DIVIDE)
    {
        ASTNode *opNode = new ASTNode({current_token.type, current_token.value, current_token.line}); // Cria um nó para o operador
        advance();                                                                                    // Avança para o próximo token (o próximo fator)
        termNode->addChild(opNode);                                                                   // Adiciona o operador (com o fator) ao nó Term
        termNode->addChild(parse_factor());                                                           // Adiciona o fator como filho do operador
    }

    return termNode;
}

// Implementação de parse_arguments
ASTNode *Parser::parse_arguments()
{
    std::cout << "Estou dentro de parse_arguments" << std::endl;
    if (current_token.type == LPAREN)
    {
        advance();
        ASTNode *argsNode = new ASTNode({NONE, "Arguments", ""});
        argsNode->addChild(parse_parameter_list());
        expect(RPAREN);
        return argsNode;
    }
    else
    {
        return new ASTNode({NONE, "NoArguments", ""});
    }
}

// Implementação de parse_parameter_list
ASTNode *Parser::parse_parameter_list()
{
    std::cout << "Estou dentro de parse_parameter_list" << std::endl;
    ASTNode *paramListNode = new ASTNode({NONE, "ParameterList", ""});
    do
    {
        paramListNode->addChild(parse_identifier_list());
        paramListNode->addChild(expect(COLON));
        paramListNode->addChild(parse_type());
        if (current_token.type == SEMICOLON)
        {
            advance();
        }
        else
        {
            break;
        }
    } while (true);
    return paramListNode;
}

// Implementação de parse_factor
ASTNode *Parser::parse_factor()
{
    std::cout << "Estou dentro de parse_factor" << std::endl;

    if (current_token.type == NUMBER || current_token.type == FLOAT_NUMBER || current_token.type == TRUE || current_token.type == FALSE)
    {
        ASTNode *factorNode = new ASTNode({current_token.type, current_token.value, current_token.line});
        advance();
        return factorNode;
    }
    else if (current_token.type == IDENTIFIER)
    {
        ASTNode *factorNode = new ASTNode({current_token.type, current_token.value, current_token.line});
        advance();

        if (current_token.type == LPAREN)
        {
            ASTNode *callNode = new ASTNode({NONE, "FunctionCall", ""});
            callNode->addChild(factorNode);
            advance();
            callNode->addChild(parse_expression_list());
            expect(RPAREN);
            return callNode;
        }

        return factorNode;
    }
    else if (current_token.type == LPAREN)
    {
        advance();
        ASTNode *exprNode = parse_expression();
        expect(RPAREN);
        return exprNode;
    }
    else if (current_token.type == NOT)
    {
        ASTNode *notNode = new ASTNode({current_token.type, current_token.value, current_token.line});
        advance();
        notNode->addChild(parse_factor());
        return notNode;
    }
    else
    {
        throw SyntaxError("Erro de sintaxe: fator inválido");
    }
}

// Implementação de token_type_to_string
std::string Parser::token_type_to_string(TokenType type)
{
    switch (type)
    {
    case PROGRAM:
        return "program";
    case IDENTIFIER:
        return "identifier";
    case SEMICOLON:
        return ";";
    case DOT:
        return ".";
    case VAR:
        return "var";
    case COLON:
        return ":";
    case COMMA:
        return ",";
    case INTEGER:
        return "integer";
    case REAL:
        return "real";
    case BEGIN:
        return "begin";
    case END:
        return "end";
    case ASSIGNMENT:
        return ":=";
    case NUMBER:
        return "number";
    case BOOLEAN:
        return "bool";
    case PLUS:
        return "+";
    case MINUS:
        return "-";
    case MULTIPLY:
        return "*";
    case DIVIDE:
        return "/";
    case LPAREN:
        return "(";
    case RPAREN:
        return ")";
    case EOF_TOKEN:
        return "EOF";
    case PROCEDURE:
        return "procedure";
    case REL_OPERATOR:
        return "= | < | > | <= | >= | <>";
    default:
        return "unknown";
    }
}

// Implementação de reserved_words_to_token
TokenType Parser::reserved_words_to_token(std::string tokenStr)
{
    if (tokenStr == "program-101" || tokenStr == "program")
        return PROGRAM;
    else if (tokenStr == "var-102" || tokenStr == "var")
        return VAR;
    else if (tokenStr == "integer-103" || tokenStr == "integer")
        return INTEGER;
    else if (tokenStr == "real-104" || tokenStr == "real")
        return REAL;
    else if (tokenStr == "boolean-105" || tokenStr == "boolean")
        return BOOLEAN;
    else if (tokenStr == "procedure-106" || tokenStr == "procedure")
        return PROCEDURE;
    else if (tokenStr == "begin-107" || tokenStr == "begin")
        return BEGIN;
    else if (tokenStr == "end-108" || tokenStr == "end")
        return END;
    else if (tokenStr == "if-109" || tokenStr == "if")
        return IF;
    else if (tokenStr == "then-110" || tokenStr == "then")
        return THEN;
    else if (tokenStr == "else-111" || tokenStr == "else")
        return ELSE;
    else if (tokenStr == "while-112" || tokenStr == "while")
        return WHILE;
    else if (tokenStr == "do-113" || tokenStr == "do")
        return DO;
    else if (tokenStr == "not-114" || tokenStr == "not")
        return NOT;
    else if (tokenStr == "true")
        return TRUE;
    else if (tokenStr == "false")
        return FALSE;
    else if (tokenStr == ";")
        return SEMICOLON;
    else if (tokenStr == ".")
        return DOT;
    else if (tokenStr == ":")
        return COLON;
    else if (tokenStr == ",")
        return COMMA;
    else if (tokenStr == "+")
        return PLUS;
    else if (tokenStr == "-")
        return MINUS;
    else if (tokenStr == "*")
        return MULTIPLY;
    else if (tokenStr == "/")
        return DIVIDE;
    else if (tokenStr == "(")
        return LPAREN;
    else if (tokenStr == ")")
        return RPAREN;
    else
        return NONE;
}
