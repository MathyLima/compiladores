#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

typedef vector<string> Regras;
typedef map<string, Regras> Gramatica;


void removerRecursaoEsquerda(Gramatica& gramatica){
    Gramatica novasProducoes;
    for(const auto& [naoTerminal,regras] :gramatica){
        Regras recursivas;
        Regras naoRecursivas;
        string novoNaoTerminal = naoTerminal + "'";

        // separando regras nao recursivas das recursivas
        for(const string& regra:regras){
            if(regra.substr(0,naoTerminal.size()) == naoTerminal){
                recursivas.push_back(regra.substr(naoTerminal.size()) + novoNaoTerminal);
            }else{
                naoRecursivas.push_back(regra + novoNaoTerminal);
            }
        }

        if(!recursivas.empty()){
            novasProducoes[naoTerminal] = naoRecursivas;
            novasProducoes[novoNaoTerminal] = recursivas;
            novasProducoes[novoNaoTerminal].push_back(" ");
        }else{
            novasProducoes[naoTerminal] = regras;
        }
    }

    gramatica = novasProducoes;
}