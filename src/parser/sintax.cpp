#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <stack>

// Estrutura de token
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

// Classe para token
struct Token
{
    TokenType type;
    std::string value;
    std::string line;
};

// Para alterar o que vai pro nó, altere a a Struct ASTNode e a função build_node_levels
struct NodeLevel
{
    std::vector<std::vector<std::string>> levels;

    void addLevel(const std::vector<std::string> &level)
    {
        levels.push_back(level);
    }

    void printLevels()
    {
        for (const auto &level : levels)
        {
            std::cout << "[";
            for (size_t i = 0; i < level.size(); ++i)
            {
                std::cout << level[i];
                if (i != level.size() - 1)
                {
                    std::cout << ", ";
                }
            }
            std::cout << "]" << std::endl;
        }
    }
};

// Exceção para erros de sintaxe
class SyntaxError : public std::runtime_error
{
public:
    SyntaxError(const std::string &msg) : std::runtime_error(msg) {}
};

// Estrutura de nó da AST
struct ASTNode
{
    std::string value;
    std::vector<ASTNode *> children;

    ASTNode(const std::string &val) : value(val) {}

    void addChild(ASTNode *child)
    {
        children.push_back(child);
    }
};

void print_ast(ASTNode *node, std::stack<std::string> &nodeStack, int indent = 0)
{
    if (!node)
        return;

    if (indent == 0)
    {
        std::cout << "Sou " << node->value << " e tenho " << node->children.size() << " filhos:" << std::endl;
        nodeStack.push(node->value); // Empilha o valor da raiz
    }

    for (ASTNode *child : node->children)
    {
        if (child == nullptr)
        {
            std::cerr << "Opa, child eh null, devo subir o nivel na arvore..." << std::endl;
            continue;
        }

        for (int i = 0; i < indent; ++i)
        {
            std::cout << "|";
        }

        std::cout << " Sou " << child->value << " e sou filho do citado acima" << std::endl;
        nodeStack.push(child->value); // Empilha o valor do filho

        print_ast(child, nodeStack, indent + 1); // Recursivamente imprime os filhos deste nó
    }
}

void build_node_levels(ASTNode *node, NodeLevel &nodeLevels, int indent = 0)
{
    if (!node)
        return;

    std::vector<std::string> currentLevel;

    // Verifica se o nó é um não-terminal
    bool isNonTerminal = node->value == "Program" || node->value == "Block" ||
                         node->value == "VarDeclaration" || node->value == "VarList" ||
                         node->value == "Subprogram" || node->value == "CompoundStatement" ||
                         node->value == "CommandList" || node->value == "AssignmentStatement" ||
                         node->value == "Expression" || node->value == "SimpleExpression" ||
                         node->value == "Term" || node->value == "IdentifierList";

    // Adiciona o nó atual (pai) como o primeiro elemento do nível somente se for um não-terminal
    if (isNonTerminal)
    {
        currentLevel.push_back(node->value);
    }

    // Adiciona os filhos ao mesmo nível
    for (ASTNode *child : node->children)
    {
        if (child == nullptr)
        {
            continue;
        }
        currentLevel.push_back(child->value);
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

// Analisador sintático
class Parser
{
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), current_token_index(0)
    {
        current_token = tokens[current_token_index];
    }

    ASTNode *parse_program()
    {
        ASTNode *root = new ASTNode("Program");
        convert_to_sintax_type();
        root->addChild(expect(PROGRAM));
        root->addChild(expect(IDENTIFIER));
        root->addChild(expect(SEMICOLON));
        root->addChild(parse_block());
        root->addChild(expect(DOT));
        return root;
    }

private:
    std::vector<Token> tokens;
    Token current_token;
    size_t current_token_index;

    void advance()
    {
        std::cout << "Avançando token: " << current_token.value << '\n';
        if (current_token_index < tokens.size() - 1)
        {
            current_token_index++;
            current_token = tokens[current_token_index];
            convert_to_sintax_type();
        }
    }

    Token peek_next_token()
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

    void convert_to_sintax_type()
    {
        if (current_token.type == KEYWORD || current_token.type == DELIMITER || current_token.type == ADD_OPERATOR || current_token.type == MULT_OPERATOR)
        {
            current_token.type = reserved_words_to_token(current_token.value);
        }
    }

    ASTNode *expect(TokenType expected_type)
    {
        if (current_token.type == expected_type)
        {
            ASTNode *node = new ASTNode(current_token.value);
            std::cout << current_token.value << '\n';
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

    ASTNode *parse_block()
    {
        std::cout << "Estou dentro de parse_block" << std::endl;
        ASTNode *blockNode = new ASTNode("Block");
        blockNode->addChild(parse_var_declaration());
        blockNode->addChild(parse_declare_subprogram());
        blockNode->addChild(parse_compound_statement());
        return blockNode;
    }

    ASTNode *parse_var_declaration()
    {
        std::cout << "Estou dentro de parse_var_declaration" << std::endl;
        ASTNode *varDeclNode = new ASTNode("VarDeclaration");
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

    ASTNode *parse_var_list_declarations()
    {
        std::cout << "Estou dentro de parse_var_list_declarations" << std::endl;
        ASTNode *varListNode = new ASTNode("VarList");

        // Primeira lista de identificadores, tipo e ';'
        varListNode->addChild(parse_identifier_list());
        varListNode->addChild(expect(COLON)); // Espera o ':'
        varListNode->addChild(parse_type());
        varListNode->addChild(expect(SEMICOLON));

        // Continua processando enquanto houver identificadores adicionais
        while (current_token.type == IDENTIFIER)
        {
            ASTNode *additionalVarList = new ASTNode("VarList");
            additionalVarList->addChild(parse_identifier_list()); // Outra lista_de_identificadores
            additionalVarList->addChild(expect(COLON));           // Espera ':'
            additionalVarList->addChild(parse_type());            // Analisa o tipo das variáveis
            additionalVarList->addChild(expect(SEMICOLON));       // Espera ';'
            varListNode->addChild(additionalVarList);
        }

        return varListNode; // Retorna o nó da lista de variáveis
    }

    ASTNode *parse_identifier_list()
    {
        std::cout << "Estou dentro de parse_identifier_list" << std::endl;
        ASTNode *idListNode = new ASTNode("IdentifierList");
        idListNode->addChild(expect(IDENTIFIER));
        while (current_token.type == COMMA)
        {
            advance();
            idListNode->addChild(expect(IDENTIFIER));
        }
        return idListNode;
    }

    ASTNode *parse_type()
    {
        std::cout << "Estou dentro de parse_type" << std::endl;
        ASTNode *typeNode = nullptr;
        if (current_token.type == INTEGER)
        {
            typeNode = new ASTNode("integer");
            advance();
        }
        else if (current_token.type == REAL)
        {
            typeNode = new ASTNode("real");
            advance();
        }
        else if (current_token.type == BOOLEAN)
        {
            typeNode = new ASTNode("boolean");
            advance();
        }
        else
        {
            throw SyntaxError("Erro de sintaxe: tipo de variável inválido");
        }
        return typeNode;
    }

    ASTNode *parse_declare_subprogram()
    {
        std::cout << "Estou dentro de parse_declare_subprogram" << std::endl;
        ASTNode *subprogramNode = new ASTNode("Subprogram");
        if (current_token.type == PROCEDURE)
        {
            subprogramNode->addChild(parse_subprogramas());
        }
        return subprogramNode;
    }

    ASTNode *parse_subprogramas()
    {
        std::cout << "Estou dentro de parse_subprogramas" << std::endl;
        ASTNode *subprogramNode = new ASTNode("Procedure");
        subprogramNode->addChild(expect(PROCEDURE));
        subprogramNode->addChild(expect(IDENTIFIER));
        subprogramNode->addChild(expect(LPAREN));
        subprogramNode->addChild(parse_identifier_list());
        subprogramNode->addChild(expect(COLON));
        subprogramNode->addChild(parse_type());
        subprogramNode->addChild(expect(RPAREN));
        subprogramNode->addChild(expect(COLON));
        subprogramNode->addChild(parse_type());
        subprogramNode->addChild(expect(SEMICOLON));
        subprogramNode->addChild(parse_declare_subprogram());
        subprogramNode->addChild(parse_compound_statement());
        return subprogramNode;
    }

    ASTNode *parse_compound_statement()
    {
        std::cout << "Estou dentro de parse_compound_statement" << std::endl;
        ASTNode *compoundNode = new ASTNode("CompoundStatement");
        compoundNode->addChild(expect(BEGIN));
        compoundNode->addChild(parse_optional_commands());
        compoundNode->addChild(expect(END));
        return compoundNode;
    }

    ASTNode *parse_optional_commands()
    {
        std::cout << "Estou dentro de parse_optional_commands" << std::endl;
        if (current_token.type == END)
        {
            return new ASTNode("Empty");
        }
        return parse_command_list();
    }

    ASTNode *parse_command_list()
    {
        std::cout << "Estou dentro de parse_command_list" << std::endl;
        ASTNode *cmdListNode = new ASTNode("CommandList");
        cmdListNode->addChild(parse_command());
        while (current_token.type == SEMICOLON)
        {
            cmdListNode->addChild(expect(SEMICOLON)); // Adiciona o ';' à AST
            cmdListNode->addChild(parse_command());
        }
        return cmdListNode;
    }

    ASTNode *parse_command()
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
                advance();
                return new ASTNode("ProcedureActivation");
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
        return nullptr;
    }

    ASTNode *parse_assignment_statement()
    {
        std::cout << "Estou dentro de parse_assignment_statement" << std::endl;
        ASTNode *assignNode = new ASTNode("AssignmentStatement");
        assignNode->addChild(expect(IDENTIFIER));
        assignNode->addChild(expect(ASSIGNMENT));
        assignNode->addChild(parse_expression());
        return assignNode;
    }

    ASTNode *parse_if_statement()
    {
        std::cout << "Estou dentro de parse_if_statement" << std::endl;
        ASTNode *ifNode = new ASTNode("IfStatement");
        ifNode->addChild(expect(IF));
        ifNode->addChild(parse_expression());
        ifNode->addChild(expect(THEN));
        ifNode->addChild(parse_command());
        ifNode->addChild(parse_else_part());
        return ifNode;
    }

    ASTNode *parse_else_part()
    {
        std::cout << "Estou dentro de parse_else_part" << std::endl;
        if (current_token.type == ELSE)
        {
            return expect(ELSE);
        }
        return new ASTNode("NoElse");
    }

    ASTNode *parse_while_statement()
    {
        std::cout << "Estou dentro de parse_while_statement" << std::endl;
        ASTNode *whileNode = new ASTNode("WhileStatement");
        whileNode->addChild(expect(WHILE));
        whileNode->addChild(parse_expression());
        whileNode->addChild(expect(DO));
        whileNode->addChild(parse_command());
        return whileNode;
    }

    ASTNode *parse_expression()
    {
        std::cout << "Estou dentro de parse_expression" << std::endl;
        ASTNode *exprNode = new ASTNode("Expression");
        exprNode->addChild(parse_simple_expression());
        if (current_token.type == REL_OPERATOR)
        {
            ASTNode *relOpNode = new ASTNode(current_token.value);
            advance();
            relOpNode->addChild(parse_simple_expression());
            exprNode->addChild(relOpNode);
        }
        return exprNode;
    }

    ASTNode *parse_simple_expression()
    {
        std::cout << "Estou dentro de parse_simple_expression" << std::endl;
        ASTNode *simpleExprNode = new ASTNode("SimpleExpression");
        if (current_token.type == PLUS || current_token.type == MINUS)
        {
            ASTNode *sinalNode = new ASTNode(current_token.value);
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
            ASTNode *addOpNode = new ASTNode(current_token.value);
            advance();
            addOpNode->addChild(parse_term());
            simpleExprNode->addChild(addOpNode);
        }
        return simpleExprNode;
    }
    ASTNode *parse_term()
    {
        std::cout << "Estou dentro de parse_term" << std::endl;
        ASTNode *termNode = new ASTNode("Term");
        termNode->addChild(parse_factor());

        while (current_token.type == MULTIPLY || current_token.type == DIVIDE)
        {
            ASTNode *opNode = new ASTNode(current_token.value); // Cria um nó para o operador
            advance();                                          // Avança para o próximo token (o próximo fator)
             termNode->addChild(opNode);                         // Adiciona o operador (com o fator) ao nó Term
            termNode->addChild(parse_factor());                   // Adiciona o fator como filho do operador
           
        }

        return termNode;
    }

    ASTNode *parse_factor()
    {
        std::cout << "Estou dentro de parse_factor" << std::endl;

        if (current_token.type == NUMBER || current_token.type == FLOAT_NUMBER || current_token.type == TRUE || current_token.type == FALSE)
        {
            ASTNode *factorNode = new ASTNode(current_token.value);
            advance();
            return factorNode;
        }
        else if (current_token.type == IDENTIFIER)
        {
            ASTNode *factorNode = new ASTNode(current_token.value);
            advance();

            if (current_token.type == LPAREN)
            {
                ASTNode *callNode = new ASTNode("ProcedureCall");
                callNode->addChild(factorNode);
                advance();
                callNode->addChild(parse_expression());
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
            ASTNode *notNode = new ASTNode(current_token.value);
            advance();
            notNode->addChild(parse_factor());
            return notNode;
        }
        else
        {
            throw SyntaxError("Erro de sintaxe: fator inválido");
        }
    }

    std::string token_type_to_string(TokenType type)
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

    TokenType reserved_words_to_token(std::string tokenStr)
    {
        if (tokenStr == "program-101")
            return PROGRAM;
        else if (tokenStr == "var-102")
            return VAR;
        else if (tokenStr == "integer-103")
            return INTEGER;
        else if (tokenStr == "real-104")
            return REAL;
        else if (tokenStr == "boolean-105")
            return BOOLEAN;
        else if (tokenStr == "procedure-106")
            return PROCEDURE;
        else if (tokenStr == "begin-107")
            return BEGIN;
        else if (tokenStr == "end-108")
            return END;
        else if (tokenStr == "if-109")
            return IF;
        else if (tokenStr == "then-110")
            return THEN;
        else if (tokenStr == "else-111")
            return ELSE;
        else if (tokenStr == "while-112")
            return WHILE;
        else if (tokenStr == "do-113")
            return DO;
        else if (tokenStr == "not-114")
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
};

int main()
{
    std::vector<Token> tokens_test = {
        {KEYWORD, "program-101", "1"},      // program
        {IDENTIFIER, "AreaRetangulo", "1"}, // AreaRetangulo
        {DELIMITER, ";", "1"},              // ;

        {KEYWORD, "var-102", "3"},   // var
        {IDENTIFIER, "base", "4"},   // base
        {DELIMITER, ",", "4"},       // ,
        {IDENTIFIER, "altura", "4"}, // altura
        {DELIMITER, ",", "4"},       // ,
        {IDENTIFIER, "area", "4"},   // area
        {DELIMITER, ":", "4"},       // :
        {KEYWORD, "real-104", "4"},  // real
        {DELIMITER, ";", "4"},       // ;

        {KEYWORD, "begin-107", "6"}, // begin
        {IDENTIFIER, "base", "7"},   // base
        {ASSIGNMENT, ":=", "7"},     // :=
        {FLOAT_NUMBER, "5.0", "7"},  // 5.0
        {DELIMITER, ";", "7"},       // ;

        {IDENTIFIER, "altura", "8"}, // altura
        {ASSIGNMENT, ":=", "8"},     // :=
        {FLOAT_NUMBER, "10.0", "8"}, // 10.0
        {DELIMITER, ";", "8"},       // ;

        {IDENTIFIER, "area", "10"},   // area
        {ASSIGNMENT, ":=", "10"},     // :=
        {IDENTIFIER, "base", "10"},   // base
        {MULT_OPERATOR, "*", "10"},   // *
        {IDENTIFIER, "altura", "10"}, // altura
        {DELIMITER, ";", "10"},       // ;

        {KEYWORD, "end-108", "12"}, // end
        {DELIMITER, ".", "12"}      // .

    };

    Parser parser(tokens_test);
    try
    {
        ASTNode *ast = parser.parse_program(); // Gera a AST
        std::cout << "Programa Pascal analisado com sucesso!" << std::endl;

        NodeLevel nodeLevels;
        build_node_levels(ast, nodeLevels); // Constrói o array de arrays
        std::cout << "\nConteúdo do array de arrays:" << std::endl;
        nodeLevels.printLevels(); // Imprime os níveis de nós
    }
    catch (const SyntaxError &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
