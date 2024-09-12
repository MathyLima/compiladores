#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Estrutura de token
enum TokenType {
    PROGRAM, IDENTIFIER, SEMICOLON, DOT, VAR, COLON, INTEGER, REAL,
    BEGIN, END, ASSIGN, NUMBER, PLUS, MINUS, MULTIPLY, DIVIDE, LPAREN, RPAREN, EOF_TOKEN
};

// Classe para token
struct Token {
    TokenType type;
    std::string value;
};

// Exceção para erros de sintaxe
class SyntaxError : public std::runtime_error {
public:
    SyntaxError(const std::string& msg) : std::runtime_error(msg) {}
};

// Analisador sintático
class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current_token_index(0) {
        current_token = tokens[current_token_index];
    }

    void parse_program() {
        expect(PROGRAM); // pela definição inicia com program
        expect(IDENTIFIER); //
        expect(SEMICOLON);
        parse_block();
        expect(DOT);
    }

private:
    std::vector<Token> tokens;
    Token current_token;
    size_t current_token_index;

    void advance() {
        if (current_token_index < tokens.size() - 1) {
            current_token_index++;
            current_token = tokens[current_token_index];
        }
    }

    void expect(TokenType expected_type) {
        if (current_token.type == expected_type) {
            advance();
        } else {
            throw SyntaxError("Erro de sintaxe: esperado " + token_type_to_string(expected_type) + " mas encontrado " + current_token.value);
        }
    }

    void parse_block() {
        parse_var_declaration();
        parse_compound_statement();
    }

    void parse_var_declaration() {
        if (current_token.type == VAR) {
            advance();
            parse_identifier_list();
            expect(COLON);
            parse_type();
            expect(SEMICOLON);
        }
    }

    void parse_identifier_list() {
        expect(IDENTIFIER);
        while (current_token.type == SEMICOLON) {
            advance();
            expect(IDENTIFIER);
        }
    }

    void parse_type() {
        if (current_token.type == INTEGER) {
            advance();
        } else if (current_token.type == REAL) {
            advance();
        } else {
            throw SyntaxError("Erro de sintaxe: tipo de variável inválido");
        }
    }

    void parse_compound_statement() {
        expect(BEGIN);
        parse_statement_list();
        expect(END);
    }

    void parse_statement_list() {
        parse_statement();
        while (current_token.type == SEMICOLON) {
            advance();
            parse_statement();
        }
    }

    void parse_statement() {
        parse_assignment_statement();
    }

    void parse_assignment_statement() {
        expect(IDENTIFIER);
        expect(ASSIGN);
        parse_expression();
    }

    void parse_expression() {
        parse_term();
        while (current_token.type == PLUS || current_token.type == MINUS) {
            advance();
            parse_term();
        }
    }

    void parse_term() {
        parse_factor();
        while (current_token.type == MULTIPLY || current_token.type == DIVIDE) {
            advance();
            parse_factor();
        }
    }

    void parse_factor() {
        if (current_token.type == IDENTIFIER || current_token.type == NUMBER) {
            advance();
        } else if (current_token.type == LPAREN) {
            advance();
            parse_expression();
            expect(RPAREN);
        } else {
            throw SyntaxError("Erro de sintaxe: fator inválido");
        }
    }

    std::string token_type_to_string(TokenType type) {
        switch (type) {
            case PROGRAM: return "program";
            case IDENTIFIER: return "identifier";
            case SEMICOLON: return ";";
            case DOT: return ".";
            case VAR: return "var";
            case COLON: return ":";
            case INTEGER: return "integer";
            case REAL: return "real";
            case BEGIN: return "begin";
            case END: return "end";
            case ASSIGN: return ":=";
            case NUMBER: return "number";
            case PLUS: return "+";
            case MINUS: return "-";
            case MULTIPLY: return "*";
            case DIVIDE: return "/";
            case LPAREN: return "(";
            case RPAREN: return ")";
            case EOF_TOKEN: return "EOF";
            default: return "unknown";
        }
    }
};

int main() {
    // Tokens simulados de um código Pascal simples: "program exemplo; var x: integer; begin x := 5; end."
    std::vector<Token> tokens = {
        {PROGRAM, "program"}, {IDENTIFIER, "exemplo"}, {SEMICOLON, ";"},
        {VAR, "var"}, {IDENTIFIER, "x"}, {COLON, ":"}, {INTEGER, "integer"}, {SEMICOLON, ";"},
        {BEGIN, "begin"}, {IDENTIFIER, "x"}, {ASSIGN, ":="}, {NUMBER, "5"}, {SEMICOLON, ";"},
        {END, "end"}, {DOT, "."}
    };

    Parser parser(tokens);

    try {
        parser.parse_program();
        std::cout << "Programa Pascal analisado com sucesso!" << std::endl;
    } catch (const SyntaxError& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
