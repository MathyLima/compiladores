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
    std::cout<<"AQUI AIIU";
    if(tokenAtribuido == NUMBER){
        tokenAtribuido = INTEGER;
    }else if(tokenAtribuido == FLOAT_NUMBER){
        tokenAtribuido = REAL;
    }
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
        if(valorTipo == NUMBER){
            valorTipo = INTEGER;
        }else if(valorTipo == FLOAT_NUMBER){
            valorTipo = REAL;
        }
    
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
    throw std::runtime_error("Variável não declarada,erro ao buscar a variável: " + nome);
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
    throw std::runtime_error("Variável não declarada, erro ao atribuir valor: " + nome);
}

// Implementação da função processarBloco

void AnalisadorSemantico::processarBloco(const std::vector<Token> &tokens)
{
    TokenType tipoAtual = NONE;
    bool constante = false;
    bool declarandoVariavel = false;
    bool declarandoNomePrograma = false;
    std::stack<Token> pilhaDeclaracao;
    Token tokenProcessado;
    bool assignmenting = false;
    bool procedure = false;
    bool comparacao = false;
    bool booleanOp = false;
    std::stack<Token> pilhaBooleana;
    std::stack<Token> pilhaOperandos;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const auto &token = tokens[i];
        std::cout << "Processando token: " << token.getText() << " (" << token.getType() << token.getRow()+ ")\n";

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
            }
            else if (assignmenting)
            {
                std::cout << "Atribuindo valor: " << token.getText() << "linha("+token.getRow()+")"+"\n";
                pilhaOperandos.push(token);
            }
            else if (booleanOp)
            {
                pilhaOperandos.push(token);
            }
            break;
        }
        

        case PROGRAM:
            declarandoNomePrograma = true;
            break;

        case PROCEDURE:
        {
            declarandoVariavel = false;
            tipoAtual = token.getType();
            std::cout << "Entrando no modo procedure linha("+token.getRow()+")\n";
            break;
        }

        case VAR:
        {
            declarandoVariavel = true;
            std::cout << "Modo de declaração de variável ativado linha("+token.getRow()+")\n";
            break;
        }

        case EQUAL_OPERATOR:
        {
            comparacao = true;
            std::cout << "Operação de comparação linha("+token.getRow()+")\n";
            break;
        }

        case BEGIN:
        {
            std::cout << "Entrando em novo escopo linha("+token.getRow()+")\n";
            declarandoVariavel = false;
            if(!procedure){
                entradaEscopo();
            }
            procedure=false;
            break;
        }

        case END:
        {
            std::cout << "Finalizando escopo linha("+token.getRow()+")\n";
            Token tokenProcessadoFinal;
            if(tokenProcessado.getType()!=NONE){
                tokenProcessadoFinal = buscarVariavel(tokenProcessado.getText());
            }
                if (pilhaOperandos.size() > 1)
                {
                    while (!pilhaOperandos.empty())
                    {
                        Token op = pilhaOperandos.top();
                        if(op.getType() == IDENTIFIER){
                            op = buscarVariavel(op.getText());
                        }
                        std::cout<<"\n\n\n"+tokenProcessadoFinal.getText();

                        std::cout<<op.getText()+"\n\n\n";
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
                        Token tokenAux = pilhaOperandos.top();
                        if(pilhaOperandos.top().getType() == IDENTIFIER){
                            tokenAux = buscarVariavel(pilhaOperandos.top().getText());
                        }
                        std::cout << "Atribuindo valor para variável: " << tokenProcessado.getText() << " linha("+token.getRow()+")\n";
                        std::cout << pilhaOperandos.top().getType();
                            if(tokenProcessadoFinal.getType() == REAL and tokenAux.getType() == INTEGER){
                                atribuirValorVariavel(tokenProcessadoFinal.getText(),tokenAux.getText(),REAL);
                            }
                        else{

                            if (checkAtribuicao(tokenProcessado.getText(), tokenAux.getType()))
                            {
                                atribuirValorVariavel(tokenProcessado.getText(),tokenAux.getText(), tokenAux.getType());
                                pilhaOperandos.pop();
                            }
                        }
                    }
                }
                assignmenting = false;
            saidaEscopo();
            break;
        }

        case NUMBER:
        case FLOAT_NUMBER:{
            TokenType tipoConvertido;
            if(token.getType() == NUMBER){
                tipoConvertido = INTEGER;
            }
            else{
                tipoConvertido = REAL;
            }
            pilhaOperandos.push(Token(tipoConvertido,token.getText()));
            break;
        }

        case IDENTIFIER:
        {
            std::cout << "Encontrado identificador: " + token.getText() + " linha(  " +token.getRow()+") \n";
            if (verificaVariavelExiste(token.getText()))
            {
                Token variavel = buscarVariavel(token.getText());
                TokenType tipoVariavelExiste = variavel.getType();
                std::cout << "Variável '" << token.getText() << "' existe no escopo. Tipo: " << variavel.getType() << " linha("+token.getRow()+") \n";
                
                if(tipoAtual == PROCEDURE || declarandoVariavel){
                    if(verificaVariavelExiste(token.getText())){
                        std::cout<<"AQUI ESTOU "<<token.getText()<<"  ";
                        if(tipoAtual == PROCEDURE){
                            throw std::runtime_error("Erro: tentativa de redeclaração da procedure "+token.getText()+"  no mesmo escopo linha: "+ token.getRow());
                        }else{
                            if(scopeStack.top().verificaVariavelExiste(token.getText())){
                                throw std::runtime_error("Erro: tentativa de redeclaração da variável "+ token.getText() + " no mesmo escopo linha "+ token.getRow());

                            }
                        }
                    }
                }
                else if (assignmenting)
                {
                    std::cout << "Atribuindo valor à variável: " << tokenProcessado.getText() << " linha("+token.getRow()+")\n";
                   
                    pilhaOperandos.push(token);
                }
                else if (booleanOp)
                {

                    pilhaOperandos.push(token);
                    
                }
                 
                else
                {
                    tokenProcessado = token;
                }
            }
            else
            {
                std::cout << "Variável '" << token.getText() << "' não existe, verificando se está sendo declarada... linha("+token.getRow()+")\n";
                
                if (declarandoVariavel)
                {
                    std::cout << "Empilhando para declaração: " << token.getText() << " linha("+token.getRow()+")\n";
                    pilhaDeclaracao.push(token);
                }
                else if(declarandoNomePrograma){
                    scopeStack.top().inserirVariavel(token.getText(),PROGRAM);
                    declarandoNomePrograma = false;
                }
                else if (tipoAtual == PROCEDURE)
                {
                    std::cout << "Declarando variável para procedimento: " << token.getText() << " linha("+token.getRow()+")\n\n\n\n\n\n";
                    declararVariavel(token.getText(), tipoAtual);
                    procedure = true;
                    declarandoVariavel = true;
                    tipoAtual = NONE;
                    entradaEscopo();
                }
                else
                {
                    throw std::runtime_error("Variável "+ token.getText() + " não existe no escopo atual linha("+token.getRow()+")");
                }
            }
            break;
        }

        // case THEN:
        //     if (booleanOp)
        //     {
        //         std::stack<Token> pilhaAux = pilhaOperandos;
        //         if (pilhaAux.empty())
        //         {
        //             throw std::runtime_error("Erro: Pilha vazia no fechamento de parênteses.");
        //         }

        //             Token tokenAnterior = pilhaAux.top();
        //             if(verificaVariavelExiste(pilhaAux.top().getText())){
        //                 tokenAnterior = buscarVariavel(pilhaAux.top().getText());
        //             }
        //             pilhaAux.pop();
        //             // Loop para verificar a sequência de tokens (operandos e operadores)
        //                 while (!pilhaAux.empty()) {
        //                     Token tokenAtual = pilhaAux.top();
        //                     if(verificaVariavelExiste(pilhaAux.top().getText())){
        //                     tokenAtual = buscarVariavel(pilhaAux.top().getText());
        //                     }
        //                     pilhaAux.pop();
        //                     std::cout<<tokenAnterior.getText();
        //                     std::cout<<"\n";
        //                     // Verifica se o token anterior era um número (real ou inteiro)
        //                     if (tokenAnterior.getType() == INTEGER || tokenAnterior.getType() == REAL) {
        //                         // Após um número, esperamos um operador relacional
        //                         if (tokenAtual.getType() != REL_OPERATOR) {
        //                             throw std::runtime_error("Erro: Operador relacional esperado após número.");
        //                         }
        //                     }
        //                     // Verifica se o token anterior era um operador relacional
        //                     else if (tokenAnterior.getType() == REL_OPERATOR) {
        //                         // Após um operador relacional, esperamos um número (real ou inteiro)
        //                         if (tokenAtual.getType() != INTEGER && tokenAtual.getType() != REAL) {
        //                             throw std::runtime_error("Erro: Número esperado após operador relacional.");
        //                         }
        //                     }
        //                     // Verifica se o token anterior era um booleano
        //                     else if (tokenAnterior.getType() == BOOLEAN) {
        //                         // Após um booleano, esperamos um operador lógico
        //                         std::cout<<"\n\n\n";
        //                         std::cout<<tokenAtual.getType();
        //                         if (tokenAtual.getType() != LOGICAL_OPERATOR) {
        //                             throw std::runtime_error("Erro: Operador lógico esperado após booleano.");
        //                         }
        //                     }
        //                     // Verifica se o token anterior era um operador lógico
        //                     else if (tokenAnterior.getType() == LOGICAL_OPERATOR) {
        //                         // Após um operador lógico, esperamos um booleano
        //                         if (tokenAtual.getType() != BOOLEAN) {
        //                             throw std::runtime_error("Erro: Booleano esperado após operador lógico.");
        //                         }
        //                     }

        //                     // Atualiza o token anterior para o próximo loop
        //                     tokenAnterior = tokenAtual;
        //                 }
        //                 booleanOp = false;
        //             }
        //     break;

        case ASSIGNMENT:
        {
            assignmenting = true;
            std::cout << "Atribuição detectada linha("+token.getRow()+")\n";
            break;
        }

        case SEMICOLON:
        {
            std::cout << "Ponto e vírgula detectado, finalizando instrução linha("+token.getRow()+")\n";
            if (assignmenting)
            {
                Token tokenProcessadoFinal = buscarVariavel(tokenProcessado.getText());
                if (pilhaOperandos.size() > 1)
                {
                    while (!pilhaOperandos.empty())
                    {
                        Token op = pilhaOperandos.top();
                        if(op.getType() == IDENTIFIER){
                            op = buscarVariavel(op.getText());
                        }
                        std::cout<<"\n\n\n"+tokenProcessadoFinal.getText();

                        std::cout<<op.getText()+"\n\n\n";
                        pilhaOperandos.pop();
                        std::cout << "Verificando tipo: " << op.getType() << "\n";
                        if (op.getType() == REAL && tokenProcessadoFinal.getType() == INTEGER)
                        {
                            throw std::runtime_error("Operações entre variáveis inválidas linha("+token.getRow()+")");
                        }
                    }
                }
                else
                {
                    if (pilhaOperandos.size() == 1)
                    {   
                        Token tokenAux = pilhaOperandos.top();
                        if(pilhaOperandos.top().getType() == IDENTIFIER){
                            tokenAux = buscarVariavel(pilhaOperandos.top().getText());
                        }
                        std::cout << "Atribuindo valor para variável: " << tokenProcessado.getText() << " linha("+token.getRow()+")\n";
                            if(tokenProcessadoFinal.getType() == REAL and tokenAux.getType() == INTEGER){
                                atribuirValorVariavel(tokenProcessadoFinal.getText(),tokenAux.getText(),REAL);
                            }
                        else{

                            if (checkAtribuicao(tokenProcessado.getText(), tokenAux.getType()))
                            {
                                atribuirValorVariavel(tokenProcessado.getText(),tokenAux.getText(), tokenAux.getType());
                                pilhaOperandos.pop();
                            }
                        }
                    }
                }
                assignmenting = false;
            }
            
            break;
        }

        case IF:
        case WHILE:{
            booleanOp = true;
            std::cout << "Estrutura condicional detectada linha("+token.getRow()+")\n";
            break;
        }

        // case DO:{
        //     booleanOp = false;

        // }

        case LOGICAL_OPERATOR:
        case REL_OPERATOR:{

            if (booleanOp)
            {
                pilhaOperandos.push(token);
                std::cout << "Operador lógico empilhado: " << token.getText() << " linha("+token.getRow()+")\n";
            }
            break;
        }
        
        case AND:
        case OR:
        case NOT:
        {
            booleanOp = true;

            pilhaOperandos.push(Token(LOGICAL_OPERATOR,token.getText(),token.getRow()));
            std::cout << "Operador lógico AND/OR detectado linha("+token.getRow()+")\n";
            break;
        }

        case RPAREN:
        {
            std::cout << "Fechamento de parênteses detectado linha("+token.getRow()+")\n";
            if (booleanOp)
            {
                std::stack<Token> pilhaAux = pilhaOperandos;
                if (pilhaAux.empty())
                {
                    throw std::runtime_error("Erro: Pilha vazia no fechamento de parênteses linha("+token.getRow()+").");
                }

                    Token tokenAnterior = pilhaAux.top();
                    if(verificaVariavelExiste(pilhaAux.top().getText())){
                        tokenAnterior = buscarVariavel(pilhaAux.top().getText());
                    }
                    pilhaAux.pop();
                    // Loop para verificar a sequência de tokens (operandos e operadores)
                        while (!pilhaAux.empty()) {
                            Token tokenAtual = pilhaAux.top();
                            if(verificaVariavelExiste(pilhaAux.top().getText())){
                                tokenAtual = buscarVariavel(pilhaAux.top().getText());
                            }
                            pilhaAux.pop();
                            std::cout<<"\nTOKEN ANTERIOR "+tokenAnterior.getText()+"\n\n";
                            std::cout<<"\nTOKEN ATUAL "+tokenAtual.getText()+"\n\n";

                            std::cout<<"\n";
                            // Verifica se o token anterior era um número (real ou inteiro)
                            if (tokenAnterior.getType() == INTEGER || tokenAnterior.getType() == REAL) {
                                // Após um número, esperamos um operador relacional
                                if (tokenAtual.getType() != REL_OPERATOR) {
                                    throw std::runtime_error("Erro: Operador relacional esperado após número linha("+token.getRow()+").");
                                }
                            }
                            // Verifica se o token anterior era um operador relacional
                            else if (tokenAnterior.getType() == REL_OPERATOR) {
                                // Após um operador relacional, esperamos um número (real ou inteiro)
                                if (tokenAtual.getType() != INTEGER && tokenAtual.getType() != REAL) {
                                    throw std::runtime_error("Erro: Número esperado após operador relacional linha("+token.getRow()+").");
                                }
                            }
                            // Verifica se o token anterior era um booleano
                            else if (tokenAnterior.getType() == BOOLEAN) {
                                // Após um booleano, esperamos um operador lógico
                                std::cout<<"\n\n\n";
                                std::cout<<tokenAtual.getType();
                                if (tokenAtual.getType() != LOGICAL_OPERATOR) {
                                    throw std::runtime_error("Erro: Operador lógico esperado após booleano linha("+token.getRow()+").");
                                }
                            }
                            // Verifica se o token anterior era um operador lógico
                            else if (tokenAnterior.getType() == LOGICAL_OPERATOR) {
                                // Após um operador lógico, esperamos um booleano
                                if (tokenAtual.getType() != BOOLEAN) {
                                    throw std::runtime_error("Erro: Booleano esperado após operador lógico linha("+token.getRow()+").");
                                }
                            }
                            

                            // Atualiza o token anterior para o próximo loop
                            tokenAnterior = tokenAtual;
                        }
                        booleanOp = false;
                        while (!pilhaOperandos.empty())
                        {
                            pilhaOperandos.pop();
                        }
                        
            }

                break;
            }

        case LPAREN:
        {
            if (procedure)
            { 
                declarandoVariavel = true;
                std::cout << "Entrada de parâmetros para procedimento linha("+token.getRow()+")\n";
            }
            if(!pilhaOperandos.empty()){
                if( pilhaOperandos.top().getType() == LOGICAL_OPERATOR){
                    pilhaOperandos.pop();
                }
            }
            break;
        }

            default: 

                break;
        }
    
    }
    std::cout<<"\n\n\n SEMANTICO BEM SUCEDIDO\n\n\n";
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
