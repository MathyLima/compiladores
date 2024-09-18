// Semantico.h

#ifndef ANALISADOR_SEMANTICO_H
#define ANALISADOR_SEMANTICO_H

#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../lexical/Token/Token.h"

// Estrutura para armazenar símbolos
struct Simbolo
{
    TokenType tipo; // Usando TokenType em vez de Tipo
    bool inicializado;
    bool constante;
    std::string valor;
};

// Estrutura para armazenar informações de funções
struct Funcao
{
    TokenType tipoRetorno;
    std::vector<TokenType> parametros;
};

// Estrutura para armazenar procedimentos
struct Procedimento
{
    std::vector<TokenType> parametros;
};

// Declaração da classe TabelaSimbolos
class TabelaSimbolos
{
private:
    std::unordered_map<std::string, Simbolo> variaveis;
    std::unordered_map<std::string, Funcao> funcoes;
    std::unordered_map<std::string, Procedimento> procedimentos;

public:
    void inserirVariavel(const std::string &nome, TokenType tipo, const std::string &valor = "", bool constante = false);
    void inserirFuncao(const std::string &nome, TokenType tipoRetorno, const std::vector<TokenType> &parametros);
    void inserirProcedimento(const std::string &nome, const std::vector<TokenType> &parametros);

    bool verificaVariavelExiste(const std::string &nome);
    bool verificaFuncaoExiste(const std::string &nome);
    bool verificaProcedimentoExiste(const std::string &nome);
    bool verificaConstante(const std::string &nome);

    TokenType getTipoVariavel(const std::string &nome);
    std::string getValorVariavel(const std::string &nome);
    Funcao getFuncao(const std::string &nome);
    Procedimento getProcedimento(const std::string &nome);

    void marcarInicializada(const std::string &nome);
    void atribuiValorVariavel(const std::string &nome, const std::string &valor, TokenType tokenAtribuido);
    bool verificarTipoValor(TokenType tipo, const std::string &valor, TokenType tokenAtribuido);
};

// Declaração da classe AnalisadorSemantico
class AnalisadorSemantico
{
private:
    std::stack<TabelaSimbolos> scopeStack;

public:
    AnalisadorSemantico();

    void entradaEscopo();
    void saidaEscopo();
    void declararVariavel(const std::string &nome, TokenType tipo, const std::string &valor = "");
    TokenType checkVariavel(const std::string &nome);
    void declararFuncao(const std::string &nome, TokenType tipoRetorno, std::vector<TokenType> &parametros);
    void finalizarFuncao();
    bool checkAtribuicao(const std::string &nome, TokenType valorTipo);
    bool checkOperacoes(TokenType tipo1, TokenType tipo2, TokenType valorTipo, const std::string &operador);

    bool verificaVariavelExiste(const std::string &nome);
    Token buscarVariavel(const std::string &nome);
    bool atribuirValorVariavel(const std::string &nome, const std::string &valor, TokenType tokenAtribuicao);

    void processarBloco(const std::vector<Token> &tokens);
};

#endif // ANALISADOR_SEMANTICO_H