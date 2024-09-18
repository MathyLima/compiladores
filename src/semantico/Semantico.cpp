// semantico.cpp

#include "Semantico.h"
#include <iostream>
#include <stack>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../lexical/Token/Token.h"

// Implementação da classe TabelaSimbolos

void TabelaSimbolos::inserirVariavel(const std::string &nome, TokenType tipo, const std::string &valor, bool constante)
{
    variaveis[nome] = {tipo, !valor.empty(), constante, valor};
}

void TabelaSimbolos::inserirFuncao(const std::string &nome, TokenType tipoRetorno, const std::vector<TokenType> &parametros)
{
    if (verificaFuncaoExiste(nome))
    {
        throw std::runtime_error("Erro: Função com o nome '" + nome + "' já existe.");
    }
    funcoes[nome] = {tipoRetorno, parametros};
}

void TabelaSimbolos::inserirProcedimento(const std::string &nome, const std::vector<TokenType> &parametros)
{
    if (verificaProcedimentoExiste(nome))
    {
        throw std::runtime_error("Erro: Procedimento com o nome '" + nome + "' já existe.");
    }
    procedimentos[nome] = {parametros};
}

bool TabelaSimbolos::verificaVariavelExiste(const std::string &nome)
{
    return variaveis.find(nome) != variaveis.end();
}

bool TabelaSimbolos::verificaFuncaoExiste(const std::string &nome)
{
    return funcoes.find(nome) != funcoes.end();
}

bool TabelaSimbolos::verificaProcedimentoExiste(const std::string &nome)
{
    return procedimentos.find(nome) != procedimentos.end();
}

bool TabelaSimbolos::verificaConstante(const std::string &nome)
{
    if (verificaVariavelExiste(nome))
    {
        return variaveis[nome].constante;
    }
    return false;
}

TokenType TabelaSimbolos::getTipoVariavel(const std::string &nome)
{
    if (verificaVariavelExiste(nome))
    {
        return variaveis[nome].tipo;
    }
    return TokenType::NONE;
}

std::string TabelaSimbolos::getValorVariavel(const std::string &nome)
{
    if (verificaVariavelExiste(nome))
    {
        return variaveis[nome].valor;
    }
    throw std::runtime_error("Variável não encontrada: " + nome);
}

Funcao TabelaSimbolos::getFuncao(const std::string &nome)
{
    if (verificaFuncaoExiste(nome))
    {
        return funcoes[nome];
    }
    throw std::runtime_error("Função não encontrada: " + nome);
}

Procedimento TabelaSimbolos::getProcedimento(const std::string &nome)
{
    if (verificaProcedimentoExiste(nome))
    {
        return procedimentos[nome];
    }
    throw std::runtime_error("Procedimento não encontrado: " + nome);
}

void TabelaSimbolos::marcarInicializada(const std::string &nome)
{
    if (verificaVariavelExiste(nome))
    {
        variaveis[nome].inicializado = true;
    }
    else
    {
        throw std::runtime_error("Variável não encontrada: " + nome);
    }
}

void TabelaSimbolos::atribuiValorVariavel(const std::string &nome, const std::string &valor, TokenType tokenAtribuido)
{
    if (verificaVariavelExiste(nome))
    {
        TokenType tipoVariavel = variaveis[nome].tipo;

        if (!verificarTipoValor(tipoVariavel, valor, tokenAtribuido))
        {
            throw std::runtime_error("Erro: Valor '" + valor + "' não é compatível com o tipo da variável '" + nome + "'.");
        }
        variaveis[nome].valor = valor;
    }
    else
    {
        throw std::runtime_error("Variável não encontrada: " + nome);
    }
}

// Função auxiliar
bool TabelaSimbolos::verificarTipoValor(TokenType tipo, const std::string &valor, TokenType tokenAtribuido)
{
    try
    {
        if (tipo != tokenAtribuido)
        {
            return false;
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// Implementação da classe AnalisadorSemantico

AnalisadorSemantico::AnalisadorSemantico()
{
    scopeStack.push(TabelaSimbolos());
}

void AnalisadorSemantico::entradaEscopo()
{
    scopeStack.push(TabelaSimbolos());
}

void AnalisadorSemantico::saidaEscopo()
{
    if (!scopeStack.empty())
    {
        if (scopeStack.size() == 1)
        {
            throw std::runtime_error("Erro: Tentativa de sair de escopo global sem um 'begin' correspondente");
        }
        else
        {
            scopeStack.pop();
        }
    }
    else
    {
        throw std::runtime_error("Erro: Tentativa de sair de um escopo inexistente");
    }
}

void AnalisadorSemantico::declararVariavel(const std::string &nome, TokenType tipo, const std::string &valor)
{
    if (scopeStack.top().verificaVariavelExiste(nome))
    {
        throw std::runtime_error("Erro: Variável já declarada no escopo atual: " + nome);
    }
    else
    {
        scopeStack.top().inserirVariavel(nome, tipo, valor);
    }
}

TokenType AnalisadorSemantico::checkVariavel(const std::string &nome)
{
    std::stack<TabelaSimbolos> tempStack = scopeStack;

    while (!tempStack.empty())
    {
        if (tempStack.top().verificaVariavelExiste(nome))
        {
            return tempStack.top().getTipoVariavel(nome);
        }
        tempStack.pop();
    }

    throw std::runtime_error("Erro: Variável '" + nome + "' não declarada: ");
}

void AnalisadorSemantico::declararFuncao(const std::string &nome, TokenType tipoRetorno, std::vector<TokenType> &parametros)
{
    if (scopeStack.top().verificaFuncaoExiste(nome))
    {
        throw std::runtime_error("Erro: Função já declarada no escopo atual: " + nome);
    }
    else
    {
        scopeStack.top().inserirFuncao(nome, tipoRetorno, parametros);
        entradaEscopo(); // Novo escopo para as variáveis da função
    }
}

void AnalisadorSemantico::finalizarFuncao()
{
    saidaEscopo(); // Saia do escopo da função ao finalizar
}

bool AnalisadorSemantico::checkAtribuicao(const std::string &nome, TokenType valorTipo)
{
    TokenType varTipo = buscarVariavel(nome).getType();

    if (scopeStack.top().verificaConstante(nome))
    {
        throw std::runtime_error("Erro: Tentativa de modificação da constante '" + nome + "'.");
    }
    if (varTipo != valorTipo)
    {
        throw std::runtime_error("Erro: Atribuição inválida para a variável '" + nome +
                                 "'. Esperado tipo: " + std::to_string(static_cast<int>(varTipo)) +
                                 ". Encontrado tipo: " + std::to_string(static_cast<int>(valorTipo)));
    }
    return true;
}

bool AnalisadorSemantico::checkOperacoes(TokenType tipo1, TokenType tipo2, TokenType valorTipo, const std::string &operador)
{
    // Implementação conforme o seu código original
    // Aqui você pode adicionar a lógica para verificar as operações
    return true; // Ajuste conforme necessário
}

bool AnalisadorSemantico::verificaVariavelExiste(const std::string &nome)
{
    std::stack<TabelaSimbolos> escoposAux = scopeStack;
    while (!escoposAux.empty())
    {
        if (escoposAux.top().verificaVariavelExiste(nome))
        {
            return true;
        }
        escoposAux.pop();
    }
    return false;
}

Token AnalisadorSemantico::buscarVariavel(const std::string &nome)
{
    std::stack<TabelaSimbolos> escoposAux = scopeStack;
    while (!escoposAux.empty())
    {
        if (escoposAux.top().verificaVariavelExiste(nome))
        {
            TokenType tipoVar = escoposAux.top().getTipoVariavel(nome);
            return Token(tipoVar, nome, "1"); // Ajuste conforme sua classe Token
        }
        escoposAux.pop();
    }
    throw std::runtime_error("Variável não declarada: " + nome);
}

bool AnalisadorSemantico::atribuirValorVariavel(const std::string &nome, const std::string &valor, TokenType tokenAtribuicao)
{
    std::stack<TabelaSimbolos> escoposAux = scopeStack;

    while (!escoposAux.empty())
    {
        TabelaSimbolos &tabela = escoposAux.top();

        if (tabela.verificaVariavelExiste(nome))
        {
            tabela.atribuiValorVariavel(nome, valor, tokenAtribuicao);
            return true; // A variável foi encontrada e o valor foi atribuído
        }

        escoposAux.pop();
    }

    // Se chegar aqui, a variável não foi encontrada
    throw std::runtime_error("Variável não declarada: " + nome);
}

// Implementação da função processarBloco

void AnalisadorSemantico::processarBloco(const std::vector<Token> &tokens)
{
    TokenType tipoAtual = NONE;
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

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const auto &token = tokens[i];
        std::cout << "Processando token: " << token.getText() << " (" << token.getType() << ")\n";

        switch (token.getType())
        {
        case INTEGER:
        case REAL:
        case LITERAL:
        case BOOLEAN:
        {
            tipoAtual = token.getType();
            std::cout << "Tipo atual definido: " << tipoAtual << "\n";

            if (declarandoVariavel)
            {
                std::cout << "Declarando variáveis...\n";
                while (!pilhaDeclaracao.empty())
                {
                    Token tokenAux = pilhaDeclaracao.top();
                    std::cout << "Declarando variável: " << tokenAux.getText() << " com tipo " << tipoAtual << "\n";
                    declararVariavel(tokenAux.getText(), token.getType());
                    pilhaDeclaracao.pop();
                }
                declarandoVariavel = false;
            }
            else if (assignmenting)
            {
                std::cout << "Atribuindo valor: " << token.getText() << "\n";
                pilhaOperandos.push(token);
            }
            else if (booleanOp)
            {
                pilhaOperandos.push(token);
            }
            break;
        }

        case PROCEDURE:
        {
            procedure = true;
            tipoAtual = token.getType();
            std::cout << "Entrando no modo procedure\n";
            break;
        }

        case VAR:
        {
            declarandoVariavel = true;
            std::cout << "Modo de declaração de variável ativado\n";
            break;
        }

        case EQUAL_OPERATOR:
        {
            comparacao = true;
            std::cout << "Operação de comparação\n";
            break;
        }

        case BEGIN:
        {
            std::cout << "Entrando em novo escopo\n";
            entradaEscopo();
            break;
        }

        case END:
        {
            std::cout << "Finalizando escopo\n";
            while (!pilhaOperandos.empty())
            {
                Token op = pilhaOperandos.top();
                pilhaOperandos.pop();
                std::cout << "Processando operando: " << op.getText() << " (" << op.getType() << ")\n";
                if (op.getType() != INTEGER && tokenProcessado.getType() == INTEGER)
                {
                    throw std::runtime_error("Operações entre variáveis inválidas");
                }
            }
            procedure = false;
            declarandoVariavel = false;
            saidaEscopo();
            break;
        }

        case IDENTIFIER:
        {
            std::cout << "Encontrado identificador: " << token.getText() << "\n";
            if (verificaVariavelExiste(token.getText()))
            {
                Token variavel = buscarVariavel(token.getText());
                TokenType tipoVariavelExiste = variavel.getType();
                std::cout << "Variável '" << token.getText() << "' existe no escopo. Tipo: " << variavel.getType() << "\n";

                if (assignmenting)
                {
                    std::cout << "Atribuindo valor à variável: " << tokenProcessado.getText() << "\n";
                    if (scopeStack.top().verificaConstante(tokenProcessado.getText()))
                    {
                        throw std::runtime_error("Erro: Tentativa de modificação da constante '" + tokenProcessado.getText() + "'.");
                    }
                    else if (checkAtribuicao(tokenProcessado.getText(), tipoVariavelExiste))
                    {
                        atribuirValorVariavel(tokenProcessado.getText(), token.getText(), tipoVariavelExiste);
                    }
                }
                else if (booleanOp)
                {
                    if (checkAtribuicao(tokenProcessado.getText(), tipoVariavelExiste))
                    {
                        pilhaOperandos.push(token);
                    }
                }
                else if (declarandoVariavel)
                {
                    throw std::runtime_error("Erro: tentativa de redeclaração de variável no mesmo escopo");
                }
                else
                {
                    tokenProcessado = token;
                }
            }
            else
            {
                std::cout << "Variável '" << token.getText() << "' não existe, verificando se está sendo declarada...\n";
                if (declarandoVariavel)
                {
                    std::cout << "Empilhando para declaração: " << token.getText() << "\n";
                    pilhaDeclaracao.push(token);
                }
                else if (procedure)
                {
                    std::cout << "Declarando variável para procedimento: " << token.getText() << "\n";
                    declararVariavel(token.getText(), tipoAtual);
                    entradaEscopo();
                }
                else
                {
                    throw std::runtime_error("Variável não existe no escopo atual");
                }
            }
            break;
        }

        case ASSIGNMENT:
        {
            assignmenting = true;
            std::cout << "Atribuição detectada\n";
            break;
        }

        case SEMICOLON:
        {
            std::cout << "Ponto e vírgula detectado, finalizando instrução\n";
            if (assignmenting)
            {
                Token tokenProcessadoFinal = buscarVariavel(tokenProcessado.getText());
                if (pilhaOperandos.size() > 1)
                {
                    while (!pilhaOperandos.empty())
                    {
                        Token op = pilhaOperandos.top();
                        pilhaOperandos.pop();
                        std::cout << "Verificando tipo: " << op.getType() << "\n";
                        if (op.getType() == REAL && tokenProcessadoFinal.getType() == INTEGER)
                        {
                            throw std::runtime_error("Operações entre variáveis inválidas");
                        }
                    }
                }
                else
                {
                    if (pilhaOperandos.size() == 1)
                    {
                        std::cout << "Atribuindo valor para variável: " << tokenProcessado.getText() << "\n";
                        if (checkAtribuicao(tokenProcessado.getText(), pilhaOperandos.top().getType()))
                        {
                            atribuirValorVariavel(tokenProcessado.getText(), pilhaOperandos.top().getText(), pilhaOperandos.top().getType());
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
            if (booleanOp)
            {
                pilhaOperandos.push(token);
                std::cout << "Operador lógico empilhado: " << token.getText() << "\n";
            }
            break;

        case AND:
        case OR:
        {
            booleanOp = true;
            std::cout << "Operador lógico AND/OR detectado\n";
            break;
        }

        case RPAREN:
        {
            std::cout << "Fechamento de parênteses detectado\n";
            if (booleanOp)
            {
                std::stack<Token> pilhaAux = pilhaOperandos;
                if (pilhaAux.empty())
                {
                    throw std::runtime_error("Erro: Pilha vazia no fechamento de parênteses.");
                }

                Token tokenAnterior = pilhaAux.top();
                if (verificaVariavelExiste(pilhaAux.top().getText()))
                {
                    tokenAnterior = buscarVariavel(pilhaAux.top().getText());
                }
                pilhaAux.pop();
                while (!pilhaAux.empty())
                {
                    Token tokenAtual = pilhaAux.top();
                    if (verificaVariavelExiste(pilhaAux.top().getText()))
                    {
                        tokenAtual = buscarVariavel(pilhaAux.top().getText());
                    }
                    pilhaAux.pop();
                    std::cout << tokenAtual.getText() << "\n";
                    if (tokenAnterior.getType() == INTEGER || tokenAnterior.getType() == REAL)
                    {
                        if (tokenAtual.getType() != LOGICAL_OPERATOR)
                        {
                            throw std::runtime_error("Erro: Operador relacional esperado após número.");
                        }
                    }
                    else if (tokenAnterior.getType() == LOGICAL_OPERATOR)
                    {
                        if (tokenAtual.getType() != INTEGER && tokenAtual.getType() != REAL)
                        {
                            throw std::runtime_error("Erro: Número esperado após operador relacional.");
                        }
                    }
                    else if (tokenAnterior.getType() == BOOLEAN)
                    {
                        if (tokenAtual.getType() != LOGICAL_OPERATOR)
                        {
                            throw std::runtime_error("Erro: Operador lógico esperado após booleano.");
                        }
                    }
                    else if (tokenAnterior.getType() == LOGICAL_OPERATOR)
                    {
                        if (tokenAtual.getType() != BOOLEAN)
                        {
                            throw std::runtime_error("Erro: Booleano esperado após operador lógico.");
                        }
                    }
                    tokenAnterior = tokenAtual;
                }
            }
            else if (procedure)
            {
                std::cout << "Saindo do modo procedure\n";
                procedure = false;
            }
            break;
        }

        case LPAREN:
        {
            if (procedure)
            {
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

// Função para criar um token
Token criarToken(TokenType tipo, const std::string &texto)
{
    return Token(tipo, texto, "1");
}

// Função para executar um teste
void executarTeste(const std::vector<Token> &tokens, bool devePassar)
{
    AnalisadorSemantico analisador;
    try
    {
        analisador.processarBloco(tokens);
        if (devePassar)
        {
            std::cout << "Teste passou!" << std::endl;
        }
        else
        {
            std::cerr << "Erro: Esperado falha, mas o teste passou." << std::endl;
        }
    }
    catch (const std::runtime_error &e)
    {
        if (devePassar)
        {
            std::cerr << "Erro: Esperado sucesso, mas ocorreu uma exceção: " << e.what() << std::endl;
        }
        else
        {
            std::cout << "Teste passou com exceção esperada: " << e.what() << std::endl;
        }
    }
}

// int main()
// {
//     // Exemplo de teste
//     std::vector<Token> testeErroTipo = {
//         // Declaração das variáveis
//         criarToken(VAR, "var"),
//         criarToken(IDENTIFIER, "x"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(IDENTIFIER, "z"),
//         criarToken(COLON, ":"),
//         criarToken(BOOLEAN, "bool"),
//         criarToken(SEMICOLON, ";"),

//         criarToken(IDENTIFIER, "z"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(BOOLEAN, "false"),
//         criarToken(SEMICOLON, ";"),

//         // Atribuições
//         criarToken(IDENTIFIER, "x"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(BOOLEAN, "true"),
//         criarToken(SEMICOLON, ";"),

//         criarToken(IDENTIFIER, "y"),
//         criarToken(ASSIGNMENT, ":="),
//         criarToken(BOOLEAN, "false"),
//         criarToken(SEMICOLON, ";"),

//         // Expressão condicional com erro de tipo
//         criarToken(IF, "if"),
//         criarToken(LPAREN, "("),
//         criarToken(IDENTIFIER, "x"),
//         criarToken(LOGICAL_OPERATOR, "=="),
//         criarToken(BOOLEAN, "true"),
//         criarToken(AND, "and"),
//         criarToken(IDENTIFIER, "y"),
//         criarToken(LOGICAL_OPERATOR, "=="),
//         criarToken(BOOLEAN, "false"),
//         criarToken(RPAREN, ")"),
//         criarToken(THEN, "then"),
//     };

//     executarTeste(testeErroTipo, false);

//     return 0;
// }
