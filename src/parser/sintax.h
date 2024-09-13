#ifndef SINTAX_H
#define SINTAX_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stack>
#include <unordered_set>
#include <functional>

// Declaração do enum TokenType
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
    FALSE
};

// Estrutura para o Token
struct Token
{
    TokenType type;
    std::string value;
    std::string line;
};

// Estrutura para NodeLevel
struct NodeLevel
{
    std::vector<std::vector<std::string>> levels;

    void addLevel(const std::vector<std::string> &level);
    void printLevels();
};

// Declaração da classe SyntaxError para exceções de sintaxe
class SyntaxError : public std::runtime_error
{
public:
    SyntaxError(const std::string &msg);
};

// Estrutura para nós da AST
struct ASTNode
{
    Token token;
    std::vector<ASTNode *> children;

    ASTNode(const Token &tok);
    void addChild(ASTNode *child);
};

// Funções auxiliares
void print_ast(ASTNode *node, std::stack<std::string> &nodeStack, int indent = 0);
void build_node_levels(ASTNode *node, NodeLevel &nodeLevels, int indent = 0);

// Declaração da classe Parser
class Parser
{
public:
    Parser(const std::vector<Token> &tokens);
    ASTNode *parse_program();

private:
    std::vector<Token> tokens;
    Token current_token;
    size_t current_token_index;

    void advance();
    Token peek_next_token();
    void convert_to_sintax_type();
    ASTNode *expect(TokenType expected_type);
    bool is_command_start(TokenType type);

    ASTNode *parse_block();
    ASTNode *parse_var_declaration();
    ASTNode *parse_var_list_declarations();
    ASTNode *parse_identifier_list();
    ASTNode *parse_type();
    ASTNode *parse_declare_subprograms();
    ASTNode *parse_subprogram();
    ASTNode *parse_compound_statement();
    ASTNode *parse_optional_commands();
    ASTNode *parse_command_list();
    ASTNode *parse_command();
    ASTNode *parse_procedure_activation();
    ASTNode *parse_expression_list();
    ASTNode *parse_assignment_statement();
    ASTNode *parse_if_statement();
    ASTNode *parse_else_part();
    ASTNode *parse_while_statement();
    ASTNode *parse_expression();
    ASTNode *parse_simple_expression();
    ASTNode *parse_term();
    ASTNode *parse_arguments();
    ASTNode *parse_parameter_list();
    ASTNode *parse_factor();

    std::string token_type_to_string(TokenType type);
    TokenType reserved_words_to_token(std::string tokenStr);
};

// Funções de análise semântica
Token parse_token(const std::string &node_str);
void semantic_analysis(const NodeLevel &nodeLevels);

#endif // SINTAX_H
