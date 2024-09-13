#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

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

// Exceção para erros de sintaxe
class SyntaxError : public std::runtime_error
{
public:
    SyntaxError(const std::string &msg) : std::runtime_error(msg) {}
};

// Analisador sintático
class Parser
{
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), current_token_index(0)
    {
        current_token = tokens[current_token_index];
    }
    // programa → program id;
    // declarações_variáveis
    // declarações_de_subprogramas
    // comando_composto
    // .
    void parse_program()
    {
        // Init
        convert_to_sintax_type();

        expect(PROGRAM);    // inicia com a palavra programa
        expect(IDENTIFIER); // depois vem um nome identificador
        expect(SEMICOLON);  // por fim ponto e vígula
        parse_block();
        expect(DOT); // Termina com um ponto
    }

private:
    std::vector<Token> tokens;
    Token current_token;
    size_t current_token_index;

    void advance()
    {
        std::cout << current_token.value << '\n';
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

    void expect(TokenType expected_type)
    {
        if (current_token.type == expected_type)
        {
            advance();
        }
        else
        {
            throw SyntaxError("Linha: " + current_token.line +
                              " Erro de sintaxe: " +
                              "Esperado: " + token_type_to_string(expected_type) + " mas encontrado " + current_token.value);
        }
    }

    void parse_block()
    {

        std::cout << "parse_block" << "\n ";
        parse_var_declaration(); // declarações de variáveis
        parse_declare_subprogram();
        parse_compound_statement(); // procedimentos
    }

    // declarações_de_subprogramas → declarações_de_subprogramas declaração_de_subprograma;| ε
    void parse_declare_subprogram()
    {
        std::cout << "parse_declare_subprogram " << current_token.type << "\n";
        if (current_token.type == PROCEDURE)
        {
            parse_subprogramas();
        }
    }

    // declaração_de_subprograma →
    // procedure id argumentos;
    // declarações_variáveis
    // declarações_de_subprogramas
    // comando_composto
    void parse_subprogramas()
    {
        std::cout << "parse_subprogramas " << current_token.type << "\n";
        expect(PROCEDURE);
        expect(IDENTIFIER);
        expect(LPAREN);
        parse_identifier_list();
        expect(COLON);
        parse_type();
        expect(RPAREN);
        expect(COLON);
        parse_type();
        expect(SEMICOLON);
        parse_declare_subprogram();
        parse_compound_statement();
    }

    // declarações_variáveis → var lista_declarações_variáveis | ε
    void parse_var_declaration()
    {
        std::cout << "parse_var_declaration" << "\n ";
        if (current_token.type == VAR)
        {
            advance();
            parse_var_list_declarations(); // avança

            // if (peek_next_token().type == IDENTIFIER)
            // {
            //     advance();
            //     parse_var_list_declarations();
            // }
        }
    }

    // lista_declarações_variáveis → lista_declarações_variáveis lista_de_identificadores : tipo; | lista_de_identificadores: tipo;
    void parse_var_list_declarations()
    {
        std::cout << "parse_var_list_declarations" << "\n ";
        parse_identifier_list(); // lista de variaveis
        expect(COLON);           // Dois pontos para atribuir o valor
        parse_type();            // tipo da(s) variaveis
        expect(SEMICOLON);

        while (current_token.type == IDENTIFIER) // Se houver mais declarações (outro identificador)
        {
            parse_identifier_list(); // Outra lista_de_identificadores
            expect(COLON);           // Espera ':'
            parse_type();            // Analisa o tipo das variáveis
            expect(SEMICOLON);       // Espera ';'
        }
    }

    // lista_de_identificadores → id | lista_de_identificadores, id
    void parse_identifier_list()
    {
        std::cout << "parse_identifier_list" << "\n ";
        expect(IDENTIFIER);
        while (current_token.type == COMMA)
        { // enquanto tiver virgula ele aceita identificadores
            advance();
            expect(IDENTIFIER);
        }
    }

    // tipo → integer | real | boolean
    void parse_type()
    {
        std::cout << "parse_type" << "\n ";
        // Pode derivar para integer | real | boolean
        if (current_token.type == INTEGER)
        {
            advance();
        }
        else if (current_token.type == REAL)
        {
            advance();
        }
        else if (current_token.type == BOOLEAN)
        {
            advance();
        }
        else
        {
            throw SyntaxError("Erro de sintaxe: tipo de variável inválido");
        }
    }

    void parse_compound_statement()
    {
        std::cout << "parse_compound_statement " << "\n";
        expect(BEGIN);             // Espera o 'begin'
        parse_optional_commands(); // Analisa os comandos opcionais
        expect(END);               // Espera o 'end'
    }

    void parse_optional_commands()
    {
        std::cout << "parse_optional_commands " << "\n";
        // comandos_opcionais → lista_de_comandos | ε
        if (current_token.type == END)
        {
            // ε, se 'end' estiver imediatamente após 'begin', não há comandos
            return;
        }

        // Caso contrário, espera uma lista de comandos
        parse_command_list();
    }

    // argumentos → (lista_de_parametros) | ε
    void parse_args()
    {
        std::cout << "parse_args" << "\n ";

        if (current_token.type == LPAREN)
        {
            parse_list_params();
            expect(RPAREN);
        }
    }

    // lista_de_parametros → lista_de_identificadores : tipo | lista_de_parametros; lista_de_identificadores : tipo
    void parse_list_params()
    {
        std::cout << "parse_list_params" << "\n ";
        parse_identifier_list();
        expect(COLON);
        parse_type();
        while (current_token.type == SEMICOLON)
        {
            advance();
            expect(COLON);
            parse_type();
        }
    }

    // lista_de_comandos → comando | lista_de_comandos; comando
    void parse_command_list()
    {
        std::cout << "parse_command_list " << "\n";

        parse_command(); // Analisa o primeiro comando

        // Enquanto houver ';', continue processando comandos
        while (current_token.type == SEMICOLON)
        {
            advance();       // Avança após o ';'
            parse_command(); // Analisa o próximo comando
        }
    }

    // comando → variável : = expressão | ativação_de_procedimento | comando_composto | if expressão then comando parte_else | while expressão do comando
    void parse_command()
    {
        std::cout << "parse_command " << "\n";

        // comando → variável := expressão
        if (current_token.type == IDENTIFIER)
        {
            // Pode ser uma atribuição ou ativação de procedimento
            if (peek_next_token().type == ASSIGNMENT)
            {
                parse_assignment_statement(); // Atribuição
            }
            else
            {
                // ativação_de_procedimento
                std::cout << "Ativação de procedimento: " << current_token.value << "\n";
                advance(); // Procedimento simples
            }
        }
        // comando → comando_composto
        else if (current_token.type == BEGIN)
        {
            parse_compound_statement(); // Comando composto
        }
        // comando → if expressão then comando parte_else
        else if (current_token.type == IF)
        {
            parse_if_statement();
        }
        // comando → while expressão do comando
        else if (current_token.type == WHILE)
        {
            parse_while_statement();
        }
        else
        {
            return;
            // // Se nenhum dos casos for aplicável, erro de sintaxe
            // throw SyntaxError("Linha: " + current_token.line +
            //                   " Erro de sintaxe: " +
            //                   "Esperado: " + token_type_to_string(expected_type) + " mas encontrado " + current_token.value);
        }
    }

    void parse_assignment_statement()
    {
        std::cout << "parse_assignment_statement " << current_token.value << "\n";
        // variável := expressão
        parse_var();
        expect(ASSIGNMENT); // Espera o ':='
        parse_expression(); // Analisa a expressão
    }

    // variável → id
    void parse_var()
    {
        std::cout << "parse_var" << "\n ";
        expect(IDENTIFIER);
    }

    // ativação_de_procedimento → id | id(lista_de_expressões)
    void parse_activate_procedure()
    {
        std::cout << "parse_activate_procedure" << "\n ";
        expect(IDENTIFIER);
        if (current_token.type == LPAREN)
        {
            advance();
            parse_list_expression();
            expect(RPAREN);
        }
    }

    // if expressão then comando parte_else
    void parse_if_statement()
    {
        std::cout << "parse_if_statement " << current_token.value << "\n";
        // if expressão then comando parte_else
        expect(IF);         // Espera o 'if'
        parse_expression(); // Analisa a expressão
        expect(THEN);       // Espera o 'then'
        parse_command();    // Analisa o comando
        parse_else_part();  // Analisa a parte 'else', se houver
    }

    // parte_else → else comando | ε
    void parse_else_part()
    {
        std::cout << "parse_else_part" << "\n ";
        if (current_token.type == ELSE)
        {
            std::cout << "parse_else_part " << current_token.value << "\n";
            advance();       // Avança o 'else'
            parse_command(); // Analisa o comando que segue o 'else'
        }
    }

    // while expressão do comando
    void parse_while_statement()
    {
        std::cout << "parse_while_statement " << "\n";
        // while expressão do comando
        expect(WHILE);      // Espera o 'while'
        parse_expression(); // Analisa a expressão
        expect(DO);         // Espera o 'do'
        parse_command();    // Analisa o comando
    }

    // lista_de_expressões → expressão | lista_de_expressões, expressão
    void parse_list_expression()
    {
        std::cout << "parse_list_expression " << "\n";
        parse_expression();
        while (current_token.type == COMMA)
        {
            parse_list_expression();
            parse_expression();
        }
    }

    // expressão → expressão_simples | expressão_simples op_relacional expressão_simples
    void parse_expression()
    {
        std::cout << "parse_expression " << "\n";
        parse_simple_expression();

        if (current_token.type == REL_OPERATOR)
        {
            advance();
            parse_simple_expression();
        }
    }

    // expressão_simples → termo | sinal termo | expressão_simples op_aditivo termo
    void parse_simple_expression()
    {
        std::cout << "parse_simple_expression " << "\n";
        if (current_token.type == PLUS || current_token.type == MINUS)
        {
            parse_sinal();
            parse_term();
        }
        else
        {
            std::cout << "Entrando no  parse_term pelo else" << "\n";
            parse_term();
        }
        while (current_token.type == PLUS || current_token.type == MINUS)
        {
            std::cout << "Entrando no current_token.type while" << current_token.value << "\n";
            advance();
            parse_term();
        }
    }

    // termo → fator | termo op_multiplicativo fator
    void parse_term()
    {
        std::cout << "parse_term " << "\n";
        parse_factor();
        while (current_token.type == MULTIPLY || current_token.type == DIVIDE)
        {
            advance();
            parse_factor();
        }
    }

    // fator → id | id(lista_de_expressões) | num_int | num_real | true | false | (expressão) | not fator
    void parse_factor()
    {
        std::cout << "parse_factor " << "\n";

        if (current_token.type == NUMBER || current_token.type == TRUE || current_token.type == FALSE)
        {
            advance();
        }
        else if (current_token.type == IDENTIFIER)
        {
            advance();

            if (current_token.type == LPAREN)
            {
                advance();
                parse_expression();
                expect(RPAREN);
            }
        }
        else if (current_token.type == LPAREN)
        {
            advance();
            parse_expression();
            expect(RPAREN);
        }
        else if (current_token.type == NOT)
        {
            advance();
            parse_factor();
        }
        else
        {
            throw SyntaxError("Erro de sintaxe: fator inválido");
        }
    }

    // sinal → + | -
    void parse_sinal()
    {
        std::cout << "parse_sinal " << "\n";
        if (current_token.type == PLUS || current_token.type == MINUS)
        {
            advance();
        }
        else
        {
            throw SyntaxError("Linha: " + current_token.line +
                              " Erro de sintaxe: " +
                              "Esperado: " + token_type_to_string(ADD_OPERATOR) + " mas encontrado " + current_token.value);
        }
    }

    // op_relacional → = | <|> | <= | >= | <>
    void parse_op_relational()
    {
        std::cout << "parse_op_relational " << "\n";
        if (current_token.type == REL_OPERATOR)
        {
            advance();
        }
        else
        {
            throw SyntaxError("Linha: " + current_token.line +
                              " Erro de sintaxe: " +
                              "Esperado: " + token_type_to_string(REL_OPERATOR) + " mas encontrado " + current_token.value);
        }
    }

    // op_multiplicativo → * |/ | and
    // void parse_sinal()
    // {
    //     if (current_token.type == MULTIPLY || current_token.type == DIVIDE)
    //     {
    //         advance();
    //     }
    //     else
    //     {
    //         throw SyntaxError("Linha: " + current_token.line +
    //                           " Erro de sintaxe: " +
    //                           "Esperado: " + token_type_to_string(MULT_OPERATOR) + " mas encontrado " + current_token.value);
    //     }
    // }

    std::string
    token_type_to_string(TokenType type)
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
        case ADD_OPERATOR:
            return "+ | -";
        case MULT_OPERATOR:
            return "* | / | and";
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
        // reserved words
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

        // Delimitators
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
    // Tokens simulados de um código Pascal simples: "program exemplo; var x: integer; begin x := 5; end."
    std::vector<Token> tokens_test = {
        {KEYWORD, "program-101", "1"},
        {IDENTIFIER, "SomaSimples", "1"},
        {DELIMITER, ";", "1"},
        {KEYWORD, "var-102", "2"},
        {IDENTIFIER, "num1", "3"},
        {DELIMITER, ",", "3"},
        {IDENTIFIER, "num2", "3"},
        {DELIMITER, ",", "3"},
        {IDENTIFIER, "resultado", "3"},
        {DELIMITER, ":", "3"},
        {KEYWORD, "integer-103", "3"},
        {DELIMITER, ";", "3"},
        {KEYWORD, "begin-107", "5"},
        {IDENTIFIER, "num1", "6"},
        {ASSIGNMENT, ":=", "6"},
        {NUMBER, "5", "6"},
        {DELIMITER, ";", "6"},
        {IDENTIFIER, "num2", "7"},
        {ASSIGNMENT, ":=", "7"},
        {NUMBER, "10", "7"},
        {DELIMITER, ";", "7"},
        {KEYWORD, "if-109", "8"},
        {IDENTIFIER, "num1", "8"},
        {REL_OPERATOR, "<", "8"},
        {IDENTIFIER, "num2", "8"},
        {KEYWORD, "then-110", "8"},
        {IDENTIFIER, "resultado", "9"},
        {ASSIGNMENT, ":=", "9"},
        {IDENTIFIER, "num1", "9"},
        {ADD_OPERATOR, "+", "9"},
        {IDENTIFIER, "num2", "9"},
        {DELIMITER, ";", "9"},
        {KEYWORD, "end-108", "10"},
        {DELIMITER, ".", "10"}};

    Parser parser(tokens_test);
    try
    {
        parser.parse_program();
        std::cout << "Programa Pascal analisado com sucesso!" << std::endl;
    }
    catch (const SyntaxError &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}