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

        void insert(const std::string &name, Tipo tipo){
            simbolos[name] = tipo;
        }
        Tipo lookUp(const std::string &name){
            if(simbolos.find(name) != simbolos.end()){
                return simbolos[name];
            }
            return Tipo::UNDEFINED;
        }

        bool contains(const std::string &name){
            return simbolos.find(name)!=simbolos.end();
        }
}

// class AnalisadorSemantico{
//     private:
//         std::stack<Token> scopeStack;
// }