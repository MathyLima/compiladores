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
        void inserirVariavel(const std::string &nome, Tipo tipo,const std::string &valor){
            variaveis[nome] = {tipo,!valor.empty(),valor};
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
    int estadoAtual;
    
    Tipo mapTokenTypeToTipo(TokenType tokenType) {
        switch (tokenType) {
            case TokenType::NUMBER: return Tipo::INT;
            case TokenType::FLOAT_NUMBER: return Tipo::FLOAT;
            case TokenType::IDENTIFIER: return Tipo::STRING;
            default: return Tipo::UNDEFINED;
        }
    }

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

        throw std::runtime_error("Erro: Variável não declarada: " + nome);
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
        if (varTipo != valorTipo) {
            throw std::runtime_error("Erro: Atribuição inválida para a variável '" + nome +
                                     "'. Esperado tipo: " + std::to_string(static_cast<int>(varTipo)) +
                                     ", mas encontrou tipo: " + std::to_string(static_cast<int>(valorTipo)));
        }
    }


        // Função para verificar operações aritméticas
        Tipo checkOpsAritmeticas(Tipo tipo1, Tipo tipo2) {
            if (tipo1 == Tipo::INT && tipo2 == Tipo::INT) return Tipo::INT;
            if (tipo1 == Tipo::FLOAT && tipo2 == Tipo::FLOAT) return Tipo::FLOAT;
            if ((tipo1 == Tipo::INT && tipo2 == Tipo::FLOAT) || (tipo1 == Tipo::FLOAT && tipo2 == Tipo::INT)) {
                return Tipo::FLOAT;  // Conversão implícita
            }

            throw std::runtime_error("Erro: Operação aritmética inválida entre os tipos: " +
                                     std::to_string(static_cast<int>(tipo1)) + " e " +
                                     std::to_string(static_cast<int>(tipo2)));
        }

        // Função para verificar operações relacionais
        void checkOpsRelacionais(Tipo tipo1, Tipo tipo2) {
            if (!((tipo1 == Tipo::INT || tipo1 == Tipo::FLOAT) && (tipo2 == Tipo::INT || tipo2 == Tipo::FLOAT))) {
                throw std::runtime_error("Erro: Operações relacionais só podem ser feitas entre tipos numéricos");
            } 
        }

        void processarToken(const Token &token,std::string){
            switch (estadoAtual)
            {
                case 0:{
                    if(TokenType::KEYWORD == token.getType()){
                        estadoAtual = 1;
                    }
                }
                //estado para declaração de variáveis
                case 1:{
                    if(scopeStack.top().verificaVariavelExiste(token.getText()) == false){
                        scopeStack.top().inserirVariavel(token.getText(),Tipo::UNDEFINED);
                        estadoAtual = 2;

                        break;
                    }else{
                        throw std::runtime_error("Erro: Variavel já declarada no escopo atual");
                    }
                }

                case 2:{
                    if(TokenType::ASSIGNMENT == token.getType()){
                        estadoAtual = 3;
                    }

                    break;
                }

                case 3:{

                }
                
                break;
                default:
                    break;
                }
            }

        };

