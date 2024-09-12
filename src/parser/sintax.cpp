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
    EOF_TOKEN
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
            std::cout << current_token.value << '\n';
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

    void parse_declare_subprogram()
    {
        std::cout << "parse_declare_subprogram " << current_token.type << "\n";
        if (current_token.type == PROCEDURE)
        {
            parse_subprogramas();
        }
    }

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

    void
    parse_var_declaration()
    {
        std::cout << "parse_var_declaration" << "\n ";
        if (current_token.type == VAR)
        {
            advance();
            parse_var_list_declarations(); // avança

            if (peek_next_token().type == IDENTIFIER)
            {
                advance();
                parse_var_list_declarations();
            }
        }
    }

    void parse_var_list_declarations()
    {
        parse_identifier_list(); // lista de variaveis
        expect(COLON);           // Dois pontos para atribuir o valor
        parse_type();            // tipo da(s) variaveis
        expect(SEMICOLON);
    }

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

    void parse_type()
    {
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

    void parse_command_list()
    {
        std::cout << "parse_command_list " << "\n";
        // lista_de_comandos → comando | lista_de_comandos; comando
        parse_command(); // Analisa o primeiro comando

        // Enquanto houver ';', continue processando comandos
        while (current_token.type == SEMICOLON)
        {
            advance();       // Avança após o ';'
            parse_command(); // Analisa o próximo comando
        }
    }

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
        expect(IDENTIFIER); // Espera a variável
        expect(ASSIGNMENT); // Espera o ':='
        parse_expression(); // Analisa a expressão
    }

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

    void parse_else_part()
    {
        if (current_token.type == ELSE)
        {
            std::cout << "parse_else_part " << current_token.value << "\n";
            advance();       // Avança o 'else'
            parse_command(); // Analisa o comando que segue o 'else'
        }
    }

    void parse_while_statement()
    {
        std::cout << "parse_while_statement " << "\n";
        // while expressão do comando
        expect(WHILE);      // Espera o 'while'
        parse_expression(); // Analisa a expressão
        expect(DO);         // Espera o 'do'
        parse_command();    // Analisa o comando
    }

    void parse_expression()
    {
        std::cout << "parse_expression " << "\n";
        parse_term();
        while (current_token.type == PLUS || current_token.type == MINUS)
        {
            advance();
            parse_term();
        }
    }

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

    void parse_factor()
    {
        std::cout << "parse_factor " << "\n";

        if (current_token.type == IDENTIFIER || current_token.type == NUMBER)
        {
            advance();
        }
        else if (current_token.type == LPAREN)
        {
            advance();
            parse_expression();
            expect(RPAREN);
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
        {IDENTIFIER, "Example", "1"},
        {DELIMITER, ";", "1"},
        {KEYWORD, "var-102", "2"},
        {IDENTIFIER, "x", "2"},
        {DELIMITER, ",", "2"},
        {IDENTIFIER, "y", "2"},
        {DELIMITER, ":", "2"},
        {KEYWORD, "integer-103", "2"},
        {DELIMITER, ";", "2"},
        {KEYWORD, "var-102", "3"},
        {IDENTIFIER, "z", "3"},
        {DELIMITER, ":", "3"},
        {KEYWORD, "real-104", "3"},
        {DELIMITER, ";", "3"},
        {KEYWORD, "begin-107", "4"},
        {IDENTIFIER, "x", "5"},
        {ASSIGNMENT, ":=", "5"},
        {NUMBER, "10", "5"},
        {DELIMITER, ";", "5"},
        {IDENTIFIER, "y", "6"},
        {ASSIGNMENT, ":=", "6"},
        {NUMBER, "20", "6"},
        {DELIMITER, ";", "6"},
        {IDENTIFIER, "z", "7"},
        {ASSIGNMENT, ":=", "7"},
        {IDENTIFIER, "x", "7"},
        {ADD_OPERATOR, "+", "7"},
        {IDENTIFIER, "y", "7"},
        {MULT_OPERATOR, "/", "7"},
        {FLOAT_NUMBER, "2.0", "7"},
        {DELIMITER, ";", "7"},
        {KEYWORD, "if-109", "8"},
        {IDENTIFIER, "x", "8"},
        {REL_OPERATOR, ">", "8"},
        {IDENTIFIER, "y", "8"},
        {KEYWORD, "then-110", "8"},
        {IDENTIFIER, "writeln", "9"},
        {DELIMITER, "(", "9"},
        {LITERAL, "'x is greater'", "9"},
        {DELIMITER, ")", "9"},
        {DELIMITER, ";", "9"},
        {KEYWORD, "end-108", "10"},
        {DELIMITER, ".", "10"}

    };

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
