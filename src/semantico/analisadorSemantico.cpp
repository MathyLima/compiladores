#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <stdexcept>


enum class Tipo {
    INT,
    FLOAT,
    BOOL,
    STRING,
    UNDEFINED
};


class TabelaSimbolos{
    private:
        std::unordered_map<std::string,Tipo> simbolos;
    public:
        //insere um simbolo na tabela
        void insert(const std::string &name, Tipo tipo){
            simbolos[name] = tipo;
        }
        //retorna o tipo da variavel
        Tipo lookUp(const std::string &name){
            if(simbolos.find(name) != simbolos.end()){
                return simbolos[name];
            }
            return Tipo::UNDEFINED;
        }
        //busca simbolo no escopo atual
        bool contains(const std::string &name){
            return simbolos.find(name)!=simbolos.end();
        }
};

class AnalisadorSemantico{
    private:
        std::stack<TabelaSimbolos> scopeStack;
    public:
        AnalisadorSemantico(){
            scopeStack.push(TabelaSimbolos());
        }

        void entradaEscopo(){
            scopeStack.push(TabelaSimbolos());
        }

        void saidaEscopo(){
            if(!scopeStack.empty()){
                scopeStack.pop();
            }
            else{
                throw std::runtime_error("Erro: Tentativa de sair de um escopo inexistente");
            }
        }

        void declararVariavel(const std::string &nome, Tipo tipo){
            if(scopeStack.top().contains(nome)){
                throw std::runtime_error("Erro: Variável já declarada no escopo atual: " + nome);
            }else{
                scopeStack.top().insert(nome,tipo);
            }
        }

        //verificar se a variavel ja foi declarada dentro ou fora do escopo
        Tipo checkVariavel(const std::string &nome){
            std::stack<TabelaSimbolos> tempStack = scopeStack;

            while(!tempStack.empty()){
                if(tempStack.top().contains(nome)){
                    return tempStack.top().lookUp(nome);
                }
                tempStack.pop();
            }

            throw std::runtime_error("Erro: Variável não declarada: "+nome);
        }

        void checkTipoVariavel(const std::string &nome,Tipo expctType){
            Tipo tipoVar = checkVariavel(nome);
            if(tipoVar!= expctType){
                throw std::runtime_error("Erro: Tipos incompatíveis na atribuição para a variável: " + nome);
            }
        }
};  