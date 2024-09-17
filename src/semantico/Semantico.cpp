#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../lexical/Token/Token.h"
// #include "Semantico.h"


struct Simbolo
{
    TokenType tipo; // Usando TokenType em vez de Tipo
    bool inicializado;
    bool constante;
    std::string valor;
};

struct Funcao
{
    TokenType tipoRetorno;
    std::vector<TokenType> parametros;
};

struct Procedimento
{
    std::vector<TokenType> parametros;
};

bool verificarTipoValor(TokenType tipo, const std::string &valor,TokenType tokenAtribuido) {
    try {
        if (tipo != tokenAtribuido) {
            return false;
        } 
        return true;
            }
        catch (...) {
            return false;
        }
}


class TabelaSimbolos{
    private:
        std::unordered_map<std::string,Simbolo> variaveis;
        std::unordered_map<std::string,Funcao> funcoes;
        std::unordered_map<std::string,Procedimento> procedimentos;
    public:
        //insere uma varivel com valor opcional na tabela
        void inserirVariavel(const std::string &nome, TokenType tipo, const std::string &valor = "", bool constante = false) {
            
            variaveis[nome] = {tipo, !valor.empty(), constante, valor};
        }


        void inserirFuncao(const std::string &nome, TokenType tipoRetorno, const std::vector<TokenType> &parametros){
            if (verificaFuncaoExiste(nome)) {
                throw std::runtime_error("Erro: Função com o nome '" + nome + "' já existe.");
            }
            funcoes[nome] = {tipoRetorno,parametros};
        }

        void inserirProcedimento(const std::string &nome, const std::vector<TokenType> &parametros){
            if (verificaProcedimentoExiste(nome)) {
                throw std::runtime_error("Erro: Procedimento com o nome '" + nome + "' já existe.");
            }
            procedimentos[nome] = {parametros};
        }

        bool verificaVariavelExiste(const std::string &nome){
            return variaveis.find(nome) != variaveis.end();
        }

        bool verificaFuncaoExiste(const std::string &nome){
            return funcoes.find(nome) != funcoes.end();
        }

        bool verificaProcedimentoExiste(const std::string &nome){
            return procedimentos.find(nome) != procedimentos.end();
        }

        bool verificaConstante(const std::string &nome) {
            if (verificaVariavelExiste(nome)){
                return variaveis[nome].constante;
            }
            return false;
        }

         // Obter tipo da variável
        TokenType getTipoVariavel(const std::string &nome) {
            if (verificaVariavelExiste(nome)) {
                return variaveis[nome].tipo;
            }
            return TokenType::NONE;
        }

        // Obter valor da variável
        std::string getValorVariavel(const std::string &nome) {
            if (verificaVariavelExiste(nome)) {
                return variaveis[nome].valor;
            }
            throw std::runtime_error("Variável não encontrada: " + nome);
        }

        Funcao getFuncao(const std::string &nome){
            if(verificaFuncaoExiste(nome)){
                return funcoes[nome];
            }
            throw std::runtime_error("Função não encontrada: " + nome);
        }

        // Obter informações do procedimento
        Procedimento getProcedimento(const std::string &nome) {
            if (verificaProcedimentoExiste(nome)) {
                return procedimentos[nome];
            }
            throw std::runtime_error("Procedimento não encontrado: " + nome);
        }

        // Marcar variável como inicializada
        void marcarInicializada(const std::string &nome) {
            if (verificaVariavelExiste(nome)) {
                variaveis[nome].inicializado = true;
            } else {
                throw std::runtime_error("Variável não encontrada: " + nome);
            }
        }

        void atribuiValorVariavel(const std::string &nome,const std::string &valor,TokenType tokenAtribuido){
            if(verificaVariavelExiste(nome)){
                TokenType tipoVariavel = variaveis[nome].tipo;
                
                std::cout<<"\n\n";
                std::cout<<tokenAtribuido;
                std::cout<<"\n\n";
                std::cout<<tipoVariavel;

                if (!verificarTipoValor(tipoVariavel, valor, tokenAtribuido)) {
                    throw std::runtime_error("Erro: Valor '" + valor + "' não é compatível com o tipo da variável '" + nome + "'.");
                }
                variaveis[nome].valor = valor;
            }else {
                throw std::runtime_error("Variável não encontrada: " + nome);
            }
        }
        
};


class AnalisadorSemantico {
private:
    std::stack<TabelaSimbolos> scopeStack;
    

public:
    AnalisadorSemantico() {
        scopeStack.push(TabelaSimbolos());
    }

    void entradaEscopo() {
        scopeStack.push(TabelaSimbolos());
    }

    void saidaEscopo() {
        if (!scopeStack.empty()) {
            if (scopeStack.size() == 1) {
                throw std::runtime_error("Erro: Tentativa de sair de escopo global sem um 'begin' correspondente");
            } else {
                scopeStack.pop();
            }
        } else {
            throw std::runtime_error("Erro: Tentativa de sair de um escopo inexistente");
        }
    }

    void declararVariavel(const std::string &nome, TokenType tipo, const std::string &valor = "") {
        if (scopeStack.top().verificaVariavelExiste(nome)) {
            throw std::runtime_error("Erro: Variável já declarada no escopo atual: " + nome);
        } else {
            scopeStack.top().inserirVariavel(nome, tipo, valor);

        }
    }

    TokenType checkVariavel(const std::string &nome) {
        std::stack<TabelaSimbolos> tempStack = scopeStack;

        while (!tempStack.empty()) {
            if (tempStack.top().verificaVariavelExiste(nome)) {
                return tempStack.top().getTipoVariavel(nome);
            }
            tempStack.pop();
        }

        throw std::runtime_error("Erro: Variável '" + nome + "' não declarada: ");
    }

    void declararFuncao(const std::string &nome, TokenType tipoRetorno, std::vector<TokenType> &parametros) {
        if (scopeStack.top().verificaFuncaoExiste(nome)) {
            throw std::runtime_error("Erro: Função já declarada no escopo atual: " + nome);
        } else {
            scopeStack.top().inserirFuncao(nome, tipoRetorno, parametros);
            entradaEscopo(); // Novo escopo para as variáveis da função
        }
    }

    void finalizarFuncao() {
        saidaEscopo(); // Saia do escopo da função ao finalizar
    }

    bool checkAtribuicao(const std::string &nome, TokenType valorTipo) {
        TokenType varTipo = buscarVariavel(nome).getType();

        std::cout<<nome;

        if (scopeStack.top().verificaConstante(nome)) {
            throw std::runtime_error("Erro: Tentativa de modificação da constante '" + nome + "'.");
        }
        if (varTipo != valorTipo) {
            throw std::runtime_error("Erro: Atribuição inválida para a variável '" + nome +
                                     "'. Esperado tipo: " + std::to_string(static_cast<int>(varTipo)) +
                                     ". Encontrado tipo: " + std::to_string(static_cast<int>(valorTipo)));
        }
        // Verifica se o valor atribuído é compatível com o tipo da variável
        if (valorTipo != varTipo) {
            throw std::runtime_error("Erro: Tipos incompatíveis no valor atribuído. Tipo esperado: " + 
                                    std::to_string(static_cast<int>(valorTipo)) +
                                    ". Tipo fornecido: " + std::to_string(static_cast<int>(varTipo)));
        }


        return true;
    }

        // Função para verificar operações relacionais e lógicas
    bool checkOperacoes(TokenType tipo1, TokenType tipo2, TokenType valorTipo, const std::string &operador) {
    
        // Operações aritméticas numéricas
        if (operador == "+" || operador == "-" || operador == "*" || operador == "/") {
            if (tipo1 == TokenType::NUMBER && tipo2 == TokenType::NUMBER && valorTipo == TokenType:: NUMBER || 
            tipo1 == TokenType::NUMBER && tipo2 == TokenType::NUMBER && valorTipo == TokenType:: FLOAT_NUMBER || 
            tipo1 == TokenType::NUMBER && tipo2 == TokenType::FLOAT_NUMBER && valorTipo == TokenType:: FLOAT_NUMBER ||
            tipo1 == TokenType::FLOAT_NUMBER && tipo2 == TokenType::NUMBER && valorTipo == TokenType:: FLOAT_NUMBER || 
            tipo1 == TokenType::FLOAT_NUMBER && tipo2 == TokenType::FLOAT_NUMBER && valorTipo == TokenType:: FLOAT_NUMBER)
            return true;
        else{
            throw std::runtime_error("Erro: Operação aritmética inválida entre os tipos: " +
                                    std::to_string(static_cast<int>(tipo1)) + " e " +
                                    std::to_string(static_cast<int>(tipo2)));
            }
        }

        // Operadores lógicos booleanos
        if (operador == "&&" || operador == "||" || operador == "and" || operador == "or") {
            if (tipo1 == TokenType::BOOLEAN && tipo2 == TokenType::BOOLEAN) {
                return true;
            } else {
                throw std::runtime_error("Erro: Operação lógica inválida entre tipos " +
                                        std::to_string(static_cast<int>(tipo1)) + " e " +
                                        std::to_string(static_cast<int>(tipo2)));
            }
        }

        // Operadores de comparação (==, !=, <, >, <=, >=)
        if (operador == "==" || operador == "!=" || operador == "<" || operador == ">" || operador == "<=" || operador == ">=") {
            if (tipo1 == tipo2) {
                return true;
            } else {
                throw std::runtime_error("Erro: Comparação inválida entre tipos " +
                                        std::to_string(static_cast<int>(tipo1)) + " e " +
                                        std::to_string(static_cast<int>(tipo2)));
            }
        }

        throw std::runtime_error("Erro: Operador '" + operador + "' não reconhecido.");
    }

    bool verificaVariavelExiste(const std::string& nome) {
        std::stack<TabelaSimbolos> escoposAux = scopeStack;
        while (!escoposAux.empty()) {
            if (escoposAux.top().verificaVariavelExiste(nome)) {
                return true;
            }
            escoposAux.pop();
        }
        return false;
    }

     Token buscarVariavel(const std::string& nome) {
        std::stack<TabelaSimbolos> escoposAux = scopeStack;
        while (!escoposAux.empty()) {
            if (escoposAux.top().verificaVariavelExiste(nome)) {
                return escoposAux.top().getTipoVariavel(nome);
            }
            escoposAux.pop();
        }
        throw std::runtime_error("Variável não declarada: " + nome);
    }
    bool atribuirValorVariavel(const std::string& nome, const std::string& valor, TokenType tokenAtribuicao) {
        std::stack<TabelaSimbolos> escoposAux = scopeStack;

        while (!escoposAux.empty()) {
            TabelaSimbolos& tabela = escoposAux.top();

            if (tabela.verificaVariavelExiste(nome)) {
                tabela.atribuiValorVariavel(nome, valor,tokenAtribuicao);
                return true; // A variável foi encontrada e o valor foi atribuído
            }

            escoposAux.pop();
        }

        // Se chegar aqui, a variável não foi encontrada
        throw std::runtime_error("Variável não declarada: " + nome);
    }

    // Função para criar um token
    Token criarToken(TokenType tipo, const std::string &texto) {
        return Token(tipo,texto,"1");
    }

    void processarBloco(const std::vector<Token> &tokens) {
    TokenType tipoAtual = NONE; // Tipo atual da variável (se definido por uma KEYWORD)
    bool constante = false;
    bool declarandoVariavel = false;
    std::stack<Token> pilhaDeclaracao;
    Token tokenProcessado;
    bool assignmenting = false;
    bool procedure = false;
    bool comparacao = false;
    bool booleanOp = false;
    std::stack<Token> pilhaOperandos;
    std::stack<Token> pilhaBooleana;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto &token = tokens[i];
        std::cout << "Processando token: " << token.getText() << " (" << token.getType() << ")\n";

        switch (token.getType()) {

            case INTEGER:
            case REAL:
            case LITERAL:
            case BOOLEAN:
            {   
                tipoAtual = token.getType();
                // pilhaOperandos.push(token);
                std::cout << "Tipo atual definido: " << tipoAtual << "\n";

                if (declarandoVariavel) {
                    std::cout << "Declarando variáveis...\n";
                    while (!pilhaDeclaracao.empty()) {
                        Token tokenAux = pilhaDeclaracao.top();
                        std::cout << "Declarando variável: " << tokenAux.getText() << " com tipo " << tipoAtual << "\n";
                        declararVariavel(tokenAux.getText(), token.getType());
                        pilhaDeclaracao.pop();
                    }
                    declarandoVariavel = false;
                } else if (assignmenting) {
                    std::cout << "Atribuindo valor: " << token.getText() << "\n";
                    pilhaOperandos.push(token);
                }
                break;
            }

            case PROCEDURE: {
                procedure = true;
                tipoAtual = token.getType();
                std::cout << "Entrando no modo procedure\n";
                break;
            }

            case VAR: {
                declarandoVariavel = true;
                std::cout << "Modo de declaração de variável ativado\n";
                break;
            }

            case EQUAL_OPERATOR: {
                comparacao = true;
                std::cout << "Operação de comparação\n";
                break;
            }

            case BEGIN: {
                std::cout << "Entrando em novo escopo\n";
                entradaEscopo();
                break;
            }

            case END: {
                std::cout << "Finalizando escopo\n";
                while (!pilhaOperandos.empty()) {
                    Token op = pilhaOperandos.top();
                    pilhaOperandos.pop();
                    std::cout << "Processando operando: " << op.getText() << " (" << op.getType() << ")\n";
                    if (op.getType() != INTEGER && tokenProcessado.getType() == INTEGER) {
                        throw std::runtime_error("Operações entre variáveis inválidas");
                    }
                }
                procedure = false;
                declarandoVariavel = false;
                saidaEscopo();
                break;
            }

            case IDENTIFIER: {
                std::cout << "Encontrado identificador: " << token.getText() << "\n";
                if (verificaVariavelExiste(token.getText())) {
                    // pilhaOperandos.push(tipoVariavel);
                    Token variavel = buscarVariavel(token.getText());
                    std::string valorVariavel = variavel.getText();
                    TokenType tipoVariavelExiste = variavel.getType();
                    std::cout << "Variável '" << token.getText() << "' existe no escopo. Tipo: " << variavel.getType() << "\n";

                    if (assignmenting) {
                        std::cout << "Atribuindo valor à variável: " << tokenProcessado.getText() << "\n";
                        if (scopeStack.top().verificaConstante(tokenProcessado.getText())) {
                            throw std::runtime_error("Erro: Tentativa de modificação da constante '" + tokenProcessado.getText() + "'.");
                        } else if (checkAtribuicao(tokenProcessado.getText(), tipoVariavelExiste)) {
                            atribuirValorVariavel(tokenProcessado.getText(),token.getText() ,tipoVariavelExiste);
                        }
                    } else if (booleanOp) {
                        if (checkAtribuicao(tokenProcessado.getText(), tipoVariavelExiste)) {
                            pilhaOperandos.push(token);
                        }
                    }  else {
                        tokenProcessado = token;
                        // pilhaOperandos.push(token);
                    }
                } else {
                    std::cout << "Variável '" << token.getText() << "' não existe, verificando se está sendo declarada...\n";
                    if (declarandoVariavel) {
                        std::cout << "Empilhando para declaração: " << token.getText() << "\n";
                        pilhaDeclaracao.push(token);
                    }
                    else if (procedure) {
                        std::cout << "Declarando variável para procedimento: " << token.getText() << "\n";
                        declararVariavel(token.getText(), tipoAtual);
                        entradaEscopo();
                    }
                    else{
                        throw std::runtime_error("Variável não existe no escopo atual");
                    }
                }
                break;
            }

            case ASSIGNMENT: {
                assignmenting = true;
                std::cout << "Atribuição detectada\n";
                break;
            }

            case SEMICOLON: {
                std::cout << "Ponto e vírgula detectado, finalizando instrução\n";
                if (assignmenting) {
                    if (pilhaOperandos.size() > 1) {
                        while (!pilhaOperandos.empty()) {
                            Token op = pilhaOperandos.top();
                            pilhaOperandos.pop();
                            std::cout << "Verificando tipo: " << op.getType() << "\n";
                            if (op.getType() == REAL && tokenProcessado.getType() == INTEGER) {
                                throw std::runtime_error("Operações entre variáveis inválidas");
                            }
                        }
                    } else {
                        if(pilhaOperandos.size() == 1){
                        std::cout << "Atribuindo valor para variável: " << tokenProcessado.getText() << "\n";
                        std::cout<<"\n \n" + pilhaOperandos.top().getText()+"\n\n";
                        if (checkAtribuicao(tokenProcessado.getText(), pilhaOperandos.top().getType())) {

                            atribuirValorVariavel(tokenProcessado.getText(), pilhaOperandos.top().getText(),pilhaOperandos.top().getType());
                        }

                        }
                    }
                    assignmenting = false;
                }
                break;
            }

            case IF:
                booleanOp = true;
                std::cout << "Estrutura condicional detectada\n";
                break;

            case LOGICAL_OPERATOR:
                // if (booleanOp) {
                //     pilhaOperandos.push(token);
                //     std::cout << "Operador lógico empilhado: " << token.getText() << "\n";
                // }
                break;

            case AND:
            case OR: {
                booleanOp = true;
                std::cout << "Operador lógico AND/OR detectado\n";
                break;
            }

            case RPAREN: {
                bool booOp = false;
                std::cout << "Fechamento de parênteses detectado\n";
                if (booleanOp) {
                    while (!pilhaOperandos.empty()) {
                        if (pilhaOperandos.top().getType() == LOGICAL_OPERATOR) {
                            booOp = true;
                        }
                        pilhaOperandos.pop();
                    }
                    if (booOp) {
                        pilhaBooleana.push(criarToken(BOOLEAN, ""));
                        std::cout << "Expressão booleana processada\n";
                    }
                    booleanOp = false;
                    booOp = false;
                } else if (procedure) {
                    std::cout << "Saindo do modo procedure\n";
                    procedure = false;
                }
                break;
            }

            case LPAREN: {
                if (procedure) {
                    declarandoVariavel = true;
                    std::cout << "Entrada de parâmetros para procedimento\n";
                }
                break;
            }

            default:
                std::cout << "Token não processado: " << token.getText() << "\n";
                break;
        }
    }
}
};



// Função para criar um token
Token criarToken(TokenType tipo, const std::string &texto) {
    return Token(tipo,texto,"1");
}

// Função para executar um teste
void executarTeste(const std::vector<Token> &tokens, bool devePassar) {
    AnalisadorSemantico analisador;
    try {
        analisador.processarBloco(tokens);
        if (devePassar) {
            std::cout << "Teste passou!" << std::endl;
        } else {
            std::cerr << "Erro: Esperado falha, mas o teste passou." << std::endl;
        }
    } catch (const std::runtime_error &e) {
        if (devePassar) {
            std::cerr << "Erro: Esperado sucesso, mas ocorreu uma exceção: " << e.what() << std::endl;
        } else {
            std::cout << "Teste passou com exceção esperada: " << e.what() << std::endl;
        }
    }
}

int main() {
//     // Teste 1: Declaração e atribuição correta
//     std::vector<Token> tokens1 = {
//         criarToken(BEGIN,"Begin"),
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "x"),
//         criarToken(COLON,":"),
//         criarToken(INTEGER, "int"),
//         criarToken(SEMICOLON,";"),
//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "10"),
//         criarToken(SEMICOLON,";"),

//         criarToken(BEGIN,"Begin"),
//         criarToken(IDENTIFIER,"x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER,"10"),
//         criarToken(SEMICOLON,";"),

//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(COLON,":"),
//         criarToken(REAL, "float"),
//         criarToken(SEMICOLON,";"),

//         criarToken(BEGIN,"Begin"),
//         criarToken(IDENTIFIER,"y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(REAL,"10"),
//         criarToken(SEMICOLON,";"),

//         criarToken(END, "End"),

//         criarToken(END, "End"),

//         criarToken(PROCEDURE,"procedure"),
//         criarToken(IDENTIFIER,"nomeProcedure"),
//         criarToken(LPAREN,"("),
//         criarToken(IDENTIFIER,"Z"),
//         criarToken(COLON,":"),
//         criarToken(INTEGER,"integer"),
        
//         criarToken(RPAREN,")"),
//         criarToken(BEGIN,"begin"),
//         criarToken(IDENTIFIER,"Z"),
//         criarToken(ASSIGNMENT,":="),
//         criarToken(REAL,"10.3"),
//         criarToken(SEMICOLON,";"),

//         criarToken(END,"end"),
        
//         criarToken(END, "End"),
//         criarToken(BEGIN,"begin"),
//         criarToken(IDENTIFIER,"Z"),

//         criarToken(END, "End"),

//     };
//     executarTeste(tokens1, true);

//     // Teste 2: Tentativa de atribuição a constante
//     std::vector<Token> tokens2 = {
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(COLON,":"),
//         criarToken(INTEGER, "int"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(REAL, "20"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(NUMBER, "30"),
//         criarToken(SEMICOLON,";"),

//     };
//     executarTeste(tokens2, false); // Deve falhar, pois 'y' é uma constante

//     // Teste 3: Verificação de erro ao sair do escopo global
//     std::vector<Token> tokens3 = {
//         criarToken(BEGIN, "begin"),
//         criarToken(END, "end"),
//         criarToken(END, "end") // Tentativa de sair do escopo global novamente
//     };
//     executarTeste(tokens3, false); // Deve falhar, pois não há um 'begin' correspondente

//     std::vector<Token> tokens4 = {
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "x"),
//         criarToken(COLON, ":"),
//         criarToken(INTEGER, "int"), // Declara x como inteiro no escopo global
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "10"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "15"),
//         criarToken(SEMICOLON,";"),

//         criarToken(BEGIN, "begin"),
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(COLON, ":"),
//         criarToken(INTEGER, "int"), // Declara y no escopo aninhado
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "20"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "25"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "30"), // Modifica x no escopo aninhado
//         criarToken(SEMICOLON,";"),

//         criarToken(END, "end"),
//         criarToken(IDENTIFIER, "x"), // Acesso à x fora do escopo aninhado
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "35"),
//         criarToken(SEMICOLON,";"),

// };

// executarTeste(tokens4, true); // Espera sucesso, se 'x' for 35 e 'y' for 25 após o teste


// std::vector<Token> tokens6 = {
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "x"),
//         criarToken(COLON, ":"),
//         criarToken(INTEGER, "int"), // Declara x como inteiro no escopo global
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "10"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "15"),
//         criarToken(SEMICOLON,";"),

//         criarToken(BEGIN, "begin"),
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(COLON, ":"),
//         criarToken(INTEGER, "int"), // Declara y no escopo aninhado
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "20"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "25"),
//         criarToken(SEMICOLON,";"),

//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "30"), // Modifica x no escopo aninhado
//         criarToken(SEMICOLON,";"),

//         criarToken(END, "end"),
//         criarToken(IDENTIFIER, "x"), // Acesso à x fora do escopo aninhado
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "35"),
//         criarToken(SEMICOLON,";"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(INTEGER, "20"),
//         criarToken(SEMICOLON,";"),

//     };
// executarTeste(tokens6, false); // Espera sucesso, se 'x' for 35 e 'y' for 25 após o teste
std::vector<Token> tokensEscopo = {
    // Declaração de variáveis no escopo global
    criarToken(VAR, "var"),
    criarToken(IDENTIFIER, "x"),
    criarToken(COLON, ":"),
    criarToken(INTEGER, "integer"), // Declara 'x' como inteiro no escopo global
    criarToken(SEMICOLON, ";"),

    criarToken(VAR, "var"),
    criarToken(IDENTIFIER, "y"),
    criarToken(COLON, ":"),
    criarToken(INTEGER, "integer"), // Declara 'y' como inteiro no escopo global
    criarToken(SEMICOLON, ";"),

    criarToken(IDENTIFIER, "x"),
    criarToken(ASSIGNMENT, ":="),
    criarToken(INTEGER, "5"), // Atribui valor 5 a 'x' no escopo global
    criarToken(SEMICOLON, ";"),

    criarToken(IDENTIFIER, "y"),
    criarToken(ASSIGNMENT, ":="),
    criarToken(INTEGER, "10"), // Atribui valor 10 a 'y' no escopo global
    criarToken(SEMICOLON, ";"),

    // Declaração de uma procedure
    criarToken(PROCEDURE, "procedure"),
    criarToken(IDENTIFIER, "escopoTeste"), // Declara a procedure 'escopoTeste'
    criarToken(LPAREN, "("),
    criarToken(IDENTIFIER, "a"),
    criarToken(COLON, ":"),
    criarToken(INTEGER, "integer"), // Declara parâmetro 'a' como inteiro
    criarToken(RPAREN, ")"),
    criarToken(SEMICOLON, ";"),


    // Variável local 'z' declarada dentro da procedure
    criarToken(VAR, "var"),
    criarToken(IDENTIFIER, "z"),
    criarToken(COLON, ":"),
    criarToken(INTEGER, "integer"), // Declara 'z' como inteiro no escopo local
    criarToken(SEMICOLON, ";"),

    criarToken(IDENTIFIER, "z"),
    criarToken(ASSIGNMENT, ":="),
    criarToken(INTEGER, "20"), // Atribui valor 20 a 'z' no escopo local
    criarToken(SEMICOLON, ";"),

    criarToken(IDENTIFIER, "x"),
    criarToken(ASSIGNMENT, ":="),
    criarToken(INTEGER, "15"), // Atribui valor 15 a 'x' (variável do escopo global)
    criarToken(SEMICOLON, ";"),

    criarToken(IDENTIFIER, "y"),
    criarToken(ASSIGNMENT, ":="),
    criarToken(IDENTIFIER, "z"), // Atribui 'y := z', verificando escopo (deve pegar valor local de 'z')
    criarToken(SEMICOLON, ";"),

    criarToken(END, "end"), // Fim da procedure
    
    criarToken(SEMICOLON, "."),

    // Chamada da procedure com argumento
    criarToken(IDENTIFIER, "escopoTeste"),
    criarToken(LPAREN, "("),
    criarToken(IDENTIFIER, "x"), // Chama a procedure 'escopoTeste' passando 'x'
    criarToken(RPAREN, ")"),
    criarToken(SEMICOLON, ";"),
    
    // Tentativa de usar a variável 'z' no escopo global (deve causar erro)
    criarToken(IDENTIFIER, "z"),
    criarToken(ASSIGNMENT, ":="),
    criarToken(INTEGER, "30"), // Erro: 'z' não é visível no escopo global
    criarToken(SEMICOLON, ";"),

    criarToken(END, "end"), // Fim do programa principal
    criarToken(DOT, ".")
};
executarTeste(tokensEscopo,false);

return 0;
}
// std::vector<Token> tokens5 = {
//     criarToken(VAR, "var"),
//     criarToken(IDENTIFIER, "x"),
//     criarToken(COLON, ":"),
//     criarToken(INTEGER, "integer"), // Declara x como inteiro no escopo global
//     criarToken(SEMICOLON, ";"),

//     criarToken(VAR, "var"),
//     criarToken(IDENTIFIER, "y"),
//     criarToken(COLON, ":"),
//     criarToken(INTEGER, "integer"), // Declara y como inteiro no escopo global
//     criarToken(SEMICOLON, ";"),

//     criarToken(IDENTIFIER, "x"),
//     criarToken(ASSIGNMENT, ":="),
//     criarToken(INTEGER, "5"), // Atribui valor 5 a x
//     criarToken(SEMICOLON, ";"),

//     criarToken(IDENTIFIER, "y"),
//     criarToken(ASSIGNMENT, ":="),
//     criarToken(INTEGER, "10"), // Atribui valor 10 a y
//     criarToken(SEMICOLON, ";"),

//     criarToken(PROCEDURE, "procedure"),
//     criarToken(IDENTIFIER, "somar"), // Declara a procedure 'somar'
//     criarToken(LPAREN, "("),
//     criarToken(IDENTIFIER, "a"),
//     criarToken(COLON, ":"),
//     criarToken(INTEGER, "integer"), // Declara parâmetro 'a' como inteiro
//     criarToken(SEMICOLON, ";"),
//     criarToken(IDENTIFIER, "b"),
//     criarToken(COLON, ":"),
//     criarToken(INTEGER, "integer"), // Declara parâmetro 'b' como inteiro
//     criarToken(RPAREN, ")"),
//     criarToken(SEMICOLON, ";"),

//     criarToken(BEGIN, "begin"), // Início do corpo da procedure
//     criarToken(VAR, "var"),
//     criarToken(IDENTIFIER, "resultado"),
//     criarToken(COLON, ":"),
//     criarToken(INTEGER, "integer"), // Declara 'resultado' como inteiro
//     criarToken(SEMICOLON, ";"),

//     criarToken(IDENTIFIER, "resultado"),
//     criarToken(ASSIGNMENT, ":="),
//     criarToken(IDENTIFIER, "a"),
//     criarToken(PLUS, "+"),
//     criarToken(IDENTIFIER, "b"), // Atribui 'resultado := a + b'
//     criarToken(SEMICOLON, ";"),

//     criarToken(END, "end"), // Fim da procedure
//     criarToken(SEMICOLON, ";"),
//     criarToken(PROCEDURE,"procedure"),
//     criarToken(IDENTIFIER, "somar"),
//     criarToken(LPAREN, "("),
//     criarToken(IDENTIFIER, "x"),
//     criarToken(COMMA, ","),
//     criarToken(IDENTIFIER, "y"), // Chamada da procedure 'somar(x, y)'
//     criarToken(RPAREN, ")"),
//     criarToken(SEMICOLON, ";"),

//     criarToken(END, "end"), // Fim do programa principal
//     criarToken(DOT, ".")
// };

// executarTeste(tokens5, true); // Espera sucesso, se 'x' for 35 e 'y' for 25 após o teste

