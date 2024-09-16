// Parser.h
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stack>
#include <unordered_set>
#include <functional>
#include "../lexical/Token/Token.h"

class SyntaxError : public std::runtime_error
{
public:
    SyntaxError(const std::string &msg) : std::runtime_error(msg) {}
};

// Estrutura de nó da AST
struct ASTNode
{
    Token token; // O nó agora contém um objeto Token
    std::vector<ASTNode *> children;

    ASTNode(const Token &tok) : token(tok) {} // Construtor para inicializar o nó com um Token

    void addChild(ASTNode *child)
    {
        children.push_back(child);
    }
};

// Para alterar o que vai para o nó, altere a Struct ASTNode e a função build_node_levels
// struct NodeLevel
// {
//     std::vector<std::vector<std::string>> levels;

//     void addLevel(const std::vector<std::string> &level)
//     {
//         levels.push_back(level);
//     }

//     void printLevels()
//     {
//         for (const auto &level : levels)
//         {
//             std::cout << "[";
//             for (size_t i = 0; i < level.size(); ++i)
//             {
//                 std::cout << level[i];
//                 if (i != level.size() - 1)
//                 {
//                     std::cout << ", ";
//                 }
//             }
//             std::cout << "]" << std::endl;
//         }
//     }
// };
struct TokenSequence
{
    std::vector<Token> tokens; // Stores the linear sequence of tokens

    void addTokens(const std::vector<Token> &newTokens)
    {
        tokens.insert(tokens.end(), newTokens.begin(), newTokens.end());
    }

    void printSequence() const
    {
        for (const auto &token : tokens)
        {
            std::cout << "Type: " << token.type
                      << ", Value: " << token.value
                      << ", Line: " << token.line << std::endl;
        }
    }
};
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
    ASTNode *parse_for_statement(); 
    ASTNode *parse_expression();
    ASTNode *parse_simple_expression();
    ASTNode *parse_term();
    ASTNode *parse_arguments();
    ASTNode *parse_parameter_list();
    ASTNode *parse_factor();

    std::string token_type_to_string(TokenType type);
    TokenType reserved_words_to_token(std::string tokenStr);
};

// Declaração das funções auxiliares
void print_ast(ASTNode *node, std::stack<std::string> &nodeStack, int indent = 0);
//void build_node_levels(ASTNode *node, NodeLevel &nodeLevels, int indent = 0);
Token parse_token(const std::string &node_str);
//void semantic_analysis(const NodeLevel &nodeLevels);
void generate_token_sequence(ASTNode *node, TokenSequence &sequence);

#endif // PARSER_H