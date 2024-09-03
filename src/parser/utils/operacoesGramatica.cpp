#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

typedef vector<string> Regras;
typedef map<string, Regras> Gramatica;

void removerRecursaoEsquerda(Gramatica& gramatica) {
    Gramatica novasProducoes;

    for (const auto& [naoTerminal, regras] : gramatica) {
        Regras recursivas;
        Regras naoRecursivas;
        string novoNaoTerminal = naoTerminal + "'";

        // Separando regras não recursivas das recursivas
        for (const string& regra : regras) {
            if (regra.rfind(naoTerminal, 0) == 0) {
                // Se a regra começa com o não-terminal, é recursiva
                recursivas.push_back(regra.substr(naoTerminal.size()) + novoNaoTerminal);
            } else {
                // Caso contrário, é não recursiva
                naoRecursivas.push_back(regra + " " + novoNaoTerminal);
            }
        }

        if (!recursivas.empty()) {
            // Adiciona as produções não recursivas para o não-terminal original
            novasProducoes[naoTerminal] = naoRecursivas;

            // Adiciona as produções recursivas para o novo não-terminal
            novasProducoes[novoNaoTerminal] = recursivas;
            novasProducoes[novoNaoTerminal].push_back(" ");  // Produção para ε (vazio)
        } else {
            // Se não há regras recursivas, mantemos as regras originais
            novasProducoes[naoTerminal] = regras;
        }
    }

    gramatica = novasProducoes;
}




Gramatica getGramatica(){
    Gramatica gramatica = {
        {"programa", {"program id ; declarações_variáveis declarações_de_subprogramas comando_composto ."}},
        {"declarações_variáveis", {"var lista_declarações_variáveis", ""}},  // ε substituído por ""
        {"lista_declarações_variáveis", {"lista_declarações_variáveis lista_de_identificadores : tipo ;", "lista_de_identificadores : tipo ;"}},
        {"lista_de_identificadores", {"id", "lista_de_identificadores , id"}},
        {"tipo", {"integer", "real", "boolean"}},
        {"declarações_de_subprogramas", {"declarações_de_subprogramas declaração_de_subprograma ;", ""}},  // ε substituído por ""
        {"declaração_de_subprograma", {"procedure id argumentos ; declarações_variáveis declarações_de_subprogramas comando_composto"}},
        {"argumentos", {"( lista_de_parametros )", ""}},  // ε substituído por ""
        {"lista_de_parametros", {"lista_de_identificadores : tipo", "lista_de_parametros ; lista_de_identificadores : tipo"}},
        {"comando_composto", {"begin comandos_opcionais end"}},
        {"comandos_opcionais", {"lista_de_comandos", ""}},  // ε substituído por ""
        {"lista_de_comandos", {"comando", "lista_de_comandos ; comando"}},
        {"comando", {"variável := expressão", "ativação_de_procedimento", "comando_composto", "if expressão then comando parte_else", "while expressão do comando"}},
        {"parte_else", {"else comando", ""}},  // ε substituído por ""
        {"variável", {"id"}},
        {"ativação_de_procedimento", {"id", "id ( lista_de_expressões )"}},
        {"lista_de_expressões", {"expressão", "lista_de_expressões , expressão"}},
        {"expressão", {"expressão_simples", "expressão_simples op_relacional expressão_simples"}},
        {"expressão_simples", {"termo", "sinal termo", "expressão_simples op_aditivo termo"}},
        {"termo", {"fator", "termo op_multiplicativo fator"}},
        {"fator", {"id", "id ( lista_de_expressões )", "num_int", "num_real", "true", "false", "( expressão )", "not fator"}},
        {"sinal", {"+", "-"}},
        {"op_relacional", {"=", "<", ">", "<=", ">=", "<>"}},
        {"op_aditivo", {"+", "-", "or"}},
        {"op_multiplicativo", {"*", "/", "and"}}
    };

    return gramatica;
}