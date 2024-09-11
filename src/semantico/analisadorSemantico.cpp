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

        // Função para verificar atribuições
        void checkAtribuicao(const std::string &nome, Tipo valorTipo) {
            Tipo varTipo = checkVariavel(nome);
            if (varTipo != valorTipo) {
                throw std::runtime_error("Erro: Atribuição inválida para a variável '" + nome +
                                         "'. Esperado tipo: " + std::to_string(static_cast<int>(varTipo)) +
                                         ", mas encontrou tipo: " + std::to_string(static_cast<int>(valorTipo)));
            } else {
                std::cout << "Atribuição válida para a variável '" << nome << "'\n";
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
            if ((tipo1 == Tipo::INT || tipo1 == Tipo::FLOAT) &&
                (tipo2 == Tipo::INT || tipo2 == Tipo::FLOAT)) {
                std::cout << "Operação relacional válida entre os tipos\n";
            } else {
                throw std::runtime_error("Erro: Operações relacionais só podem ser feitas entre tipos numéricos");
            }
        }
};  