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
        }
    }

    void expect(TokenType expected_type)
    {
        if (current_token.type == KEYWORD || current_token.type == DELIMITER || current_token.type == ADD_OPERATOR || current_token.type == MULT_OPERATOR)
        {
            current_token.type = reserved_words_to_token(current_token.value);
        }
        if (current_token.type == expected_type)
        {
            advance();
        }
        else
        {
            throw SyntaxError("Erro de sintaxe: esperado " + token_type_to_string(expected_type) + " mas encontrado " + current_token.value);
        }
    }

    void parse_block()
    {
        parse_var_declaration(); // declarações de variáveis
        parse_declare_subprogram();
        parse_compound_statement(); // procedimentos
    }

    void parse_declare_subprogram()
    {
        if (current_token.type == PROCEDURE)
        {
            parse_subprogramas();
        }
    }

    void parse_subprogramas()
    {
        expect(PROCEDURE);
        expect(IDENTIFIER);
        expect(LPAREN);
        parse_identifier_list();
        expect(RPAREN);
        expect(SEMICOLON);
        parse_declare_subprogram();
        parse_compound_statement();
    }

    void
    parse_var_declaration()
    {
        printf("Estou analisando declarações de variáveis\n");
        if (current_token.type == VAR)
        {
            printf("é VAR\n");       // se tem var deve iniciar uma lista de declarações de identificadores
            advance();               // avança
            parse_identifier_list(); // lista de variaveis
            expect(COLON);           // Dois pontos para atribuir o valor
            parse_type();            // tipo da(s) variaveis
            expect(SEMICOLON);
        }
    }

    void parse_identifier_list()
    {
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
        expect(BEGIN);
        parse_statement_list();
        expect(END);
    }

    void parse_statement_list()
    {
        parse_statement();
        while (current_token.type == SEMICOLON)
        {
            advance();
            parse_statement();
        }
    }

    void parse_statement()
    {
        parse_assignment_statement();
    }

    void parse_assignment_statement()
    {
        expect(IDENTIFIER);
        expect(ASSIGNMENT);
        parse_expression();
    }

    void parse_expression()
    {
        parse_term();
        while (current_token.type == PLUS || current_token.type == MINUS)
        {
            advance();
            parse_term();
        }
    }

    void parse_term()
    {
        parse_factor();
        while (current_token.type == MULTIPLY || current_token.type == DIVIDE)
        {
            advance();
            parse_factor();
        }
    }

    void parse_factor()
    {
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
        {KEYWORD, "program-101"},
        {IDENTIFIER, "Porgrama da analaura"},
        {SEMICOLON, ";"},
        {KEYWORD, "var-102"},
        {KEYWORD, "integer-103"},
        {KEYWORD, "real-104"},
        {KEYWORD, "boolean-105"},
        {KEYWORD, "procedure-106"},
        {KEYWORD, "begin-107"},
        {KEYWORD, "end-108"},
        {KEYWORD, "if-109"},
        {KEYWORD, "then-110"},
        {KEYWORD, "else-111"},
        {KEYWORD, "while-112"},
        {KEYWORD, "do-113"},
        {KEYWORD, "not-114"},
        {IDENTIFIER, "var1"},
        {ASSIGNMENT, ":="},
        {NUMBER, "123"},
        {ADD_OPERATOR, "+"},
        {NUMBER, "456"},
        {ADD_OPERATOR, "-"},
        {NUMBER, "789"},
        {MULT_OPERATOR, "*"},
        {NUMBER, "10"},
        {MULT_OPERATOR, "/"},
        {NUMBER, "5"},
        {MULT_OPERATOR, "and"},
        {ADD_OPERATOR, "or"},
        {REL_OPERATOR, ">"},
        {REL_OPERATOR, "<"},
        {REL_OPERATOR, ">="},
        {REL_OPERATOR, "<="},
        {EQUAL_OPERATOR, "="},
        {REL_OPERATOR, "<>"},
        {DELIMITER, "("},
        {IDENTIFIER, "this"},
        {IDENTIFIER, "is"},
        {IDENTIFIER, "a"},
        {IDENTIFIER, "test"},
        {DELIMITER, ")"},
        {FLOAT_NUMBER, "123.456"}};

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
