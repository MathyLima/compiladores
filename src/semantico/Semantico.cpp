#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../lexical/Token/Token.h"
#include "Semantico.h"




struct Funcao
{
    TokenType tipoRetorno;
    std::vector<TokenType> parametros;
};

struct Procedimento
{
    std::vector<TokenType> parametros;
};

bool verificarTipoValor(TokenType tipo, const std::string &valor) {
    try {
        if (tipo == TokenType::NUMBER) {
            std::stoi(valor); // Tenta converter para inteiro
        } else if (tipo == TokenType::FLOAT_NUMBER) {
            std::stof(valor); // Tenta converter para float
        } else if (tipo == TokenType::BOOLEAN) {
            if (valor != "true" && valor != "false") {
                throw std::runtime_error("Valor booleano inválido: " + valor);
            }
            } else if (tipo == TokenType::LITERAL) {
                    // Strings são sempre válidas
            } else {
                return false; // Tipo não suportado
            }
            return true;
        } catch (...) {
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
            if (constante && !verificarTipoValor(tipo, valor)) {
                throw std::runtime_error("Erro: Valor '" + valor + "' não corresponde ao tipo da constante '" + nome + "'.");
            }
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
            throw std::runtime_error("Erro: Variável não encontrada: " + nome);
        }

        void verificaInicializacao(const std::string &nome) {
            if (verificaVariavelExiste(nome) && !variaveis[nome].inicializado) {
                throw std::runtime_error("Erro: Variável não inicializada: " + nome);
            }
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

        void atribuiValorVariavel(const std::string &nome,const std::string &valor){
            if(verificaVariavelExiste(nome)){
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

    bool checkAtribuicao(const std::string &nome, TokenType valorTipo, const std::string &valor) {
        TokenType varTipo = checkVariavel(nome);
        if (scopeStack.top().verificaConstante(nome)) {
            throw std::runtime_error("Erro: Tentativa de modificação da constante '" + nome + "'.");
        }
        if (varTipo != valorTipo) {
            throw std::runtime_error("Erro: Atribuição inválida para a variável '" + nome +
                                     "'. Esperado tipo: " + std::to_string(static_cast<int>(varTipo)) +
                                     ". Encontrado tipo: " + std::to_string(static_cast<int>(valorTipo)));
        }
        // Verifica se o valor atribuído é compatível com o tipo da variável
        if (!verificarTipoValor(valorTipo, valor)) {
            throw std::runtime_error("Erro: Valor '" + valor + "' não corresponde ao tipo da variável '" + nome + "'.");
        }

        scopeStack.top().verificaInicializacao(nome);

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


    void processarBloco(const std::vector<Token> &tokens) {
        TokenType tipoAtual = NONE; // Tipo atual da variável (se definido por uma KEYWORD)
        bool constante = false;
        Token tokenProcessado;
        bool assignmenting = false;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto &token = tokens[i];
            switch (token.getType()) {
                case INTEGER:
                case REAL:
                case BOOLEAN:{
                    tipoAtual = token.getType();
                    break;
                }

                case BEGIN:{
                    entradaEscopo();
                    break;
                }
                case END:{
                    saidaEscopo();
                    break;
                }

                case IDENTIFIER: {
                    if(scopeStack.top().verificaVariavelExiste(token.getText())){
                        std::string valorVariavel = scopeStack.top().getValorVariavel(token.getText());
                        if(assignmenting){
                            if (scopeStack.top().verificaConstante(token.getText())) {
                                throw std::runtime_error("Erro: Tentativa de modificação da constante '" + token.getText() + "'.");
                            }
                            if(checkAtribuicao(tokenProcessado.getText(),token.getType(),token.getText())){
                                scopeStack.top().atribuiValorVariavel(tokenProcessado.getText(),valorVariavel);
                                assignmenting = false;
                            };
                        }
                        else{
                            tokenProcessado = token;
                        }
                    }else{
                        if(tipoAtual != NONE){
                            scopeStack.top().inserirVariavel(token.getText(),token.getType());
                        }else{
                            throw std::runtime_error("Tentativa de chamada de variável não declarada, na linha: "+token.getRow());
                        }
                    }
                    constante = false;
                    break;
                }

                case NUMBER:
                case FLOAT_NUMBER:
                case LITERAL:{
                    if(assignmenting){
                        if(checkAtribuicao(tokenProcessado.getText(),token.getType(),token.getText())){
                            scopeStack.top().atribuiValorVariavel(tokenProcessado.getText(),token.getText());
                        }
                    }

                    break;
                }

                case ASSIGNMENT:{
                    assignmenting = true;
                    break;
                }

                
            }
        }       
    }
};