#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../lexical/Token/Token.h"


enum class Tipo {
    INT,
    FLOAT,
    BOOL,
    STRING,
    VOID,
    UNDEFINED
};

struct Simbolo
{
    Tipo tipo;
    bool inicializado;
    bool constante;
    std::string valor;
};


struct Funcao
{
    Tipo tipoRetorno;
    std::vector<Tipo> parametros;
};

struct Procedimento
{
    std::vector<Tipo> parametros;
};



class TabelaSimbolos{
    private:
        std::unordered_map<std::string,Simbolo> variaveis;
        std::unordered_map<std::string,Funcao> funcoes;
        std::unordered_map<std::string,Procedimento> procedimentos;
    public:
        //insere uma varivel com valor opcional na tabela
        void inserirVariavel(const std::string &nome, Tipo tipo, const std::string &valor, bool constante = false) {
            variaveis[nome] = {tipo, !valor.empty(), constante, valor};
        }

        void inserirFuncao(const std::string &nome, Tipo tipoRetorno, const std::vector<Tipo> &parametros){
            funcoes[nome] = {tipoRetorno,parametros};
        }

        void inserirProcedimento(const std::string &nome, const std::vector<Tipo> &parametros){
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
            if (verificaVariavelExiste(nome))
                return variaveis[nome].constante;
        }

         // Obter tipo da variável
        Tipo getTipoVariavel(const std::string &nome) {
            if (verificaVariavelExiste(nome)) {
                return variaveis[nome].tipo;
            }
            return Tipo::UNDEFINED;
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
            if (procedimentos.find(nome) != procedimentos.end()) {
                return procedimentos[nome];
            }
            throw std::runtime_error("Procedimento não encontrado: " + nome);
        }

        // Marcar variável como inicializada
        void marcarInicializada(const std::string &nome) {
            if (variaveis.find(nome) != variaveis.end()) {
                variaveis[nome].inicializado = true;
            } else {
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
            scopeStack.pop();
        } else {
            throw std::runtime_error("Erro: Tentativa de sair de um escopo inexistente");
        }
    }

    void declararVariavel(const std::string &nome, Tipo tipo, const std::string &valor = "") {
        if (scopeStack.top().verificaVariavelExiste(nome)) {
            throw std::runtime_error("Erro: Variável já declarada no escopo atual: " + nome);
        } else {
            scopeStack.top().inserirVariavel(nome, tipo, valor);
        }
    }

    Tipo checkVariavel(const std::string &nome) {
        std::stack<TabelaSimbolos> tempStack = scopeStack;

        while (!tempStack.empty()) {
            if (tempStack.top().verificaVariavelExiste(nome)) {
                return tempStack.top().getTipoVariavel(nome);
            }
            tempStack.pop();
        }

        throw std::runtime_error("Erro: Variável '" + nome + "' não declarada: ");
    }

    void declararFuncao(const std::string &nome, Tipo tipoRetorno, std::vector<Tipo> &parametros) {
        if (scopeStack.top().verificaFuncaoExiste(nome)) {
            throw std::runtime_error("Erro: Função já declarada no escopo atual: " + nome);
        } else {
            Tipo info = {tipoRetorno};
            scopeStack.top().inserirFuncao(nome, info,parametros);
            entradaEscopo(); // Novo escopo para as variáveis da função
        }
    }

    void finalizarFuncao() {
        saidaEscopo(); // Saia do escopo da função ao finalizar
    }


    void checkAtribuicao(const std::string &nome, Tipo valorTipo) {
        Tipo varTipo = checkVariavel(nome);
        if (scopeStack.top().verificaConstante(nome)) {
            throw std::runtime_error("Erro: Tentativa de modificação da constante '" + nome + "'.");
        }
        if (varTipo != valorTipo) {
            throw std::runtime_error("Erro: Atribuição inválida para a variável '" + nome +
                                    "'. Esperado tipo: " + std::to_string(static_cast<int>(varTipo)) +
                                    ", Encontrado tipo: " + std::to_string(static_cast<int>(valorTipo)));
        }
    }


        // Função para verificar operações relacionais e lógicas
    bool checkOperacoes(Tipo tipo1, Tipo tipo2, Tipo valorTipo, const std::string &operador) {
    
        // Operações aritméticas numéricas
        if (operador == "+" || operador == "-" || operador == "*" || operador == "/") {
            if (tipo1 == Tipo::INT && tipo2 == Tipo::INT && valorTipo == Tipo:: INT || 
            tipo1 == Tipo::INT && tipo2 == Tipo::INT && valorTipo == Tipo:: FLOAT || 
            tipo1 == Tipo::INT && tipo2 == Tipo::FLOAT && valorTipo == Tipo:: FLOAT ||
            tipo1 == Tipo::FLOAT && tipo2 == Tipo::INT && valorTipo == Tipo:: FLOAT || 
            tipo1 == Tipo::FLOAT && tipo2 == Tipo::FLOAT && valorTipo == Tipo:: FLOAT)
            return true;
        else{
            throw std::runtime_error("Erro: Operação aritmética inválida entre os tipos: " +
                                    std::to_string(static_cast<int>(tipo1)) + " e " +
                                    std::to_string(static_cast<int>(tipo2)));
            }
        }

        // Operadores lógicos booleanos
        if (operador == "&&" || operador == "||" || operador == "and" || operador == "or") {
            if (tipo1 == Tipo::BOOL && tipo2 == Tipo::BOOL) {
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


        void processarToken(const Token &token){
            switch (token.getType())
            {
            case TokenType::IDENTIFIER:{
                Tipo tipo = checkVariavel(token.getText());
                    if(tipo == Tipo::UNDEFINED){
                        throw std::runtime_error("Erro: Variável não declarada: "+ token.getText());
                    }
                }
                break;
                default:
                    break;
                }
            }
            

        };

