#include "Scanner.h"
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <vector>

Scanner::Scanner(const std::string &source) : pos(0), row(0), col(0)
{
    std::ifstream file(source);
    if (file.is_open())
    {
        std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        sourceBuffer = std::vector<char>(buffer.begin(), buffer.end());
        file.close();
    }
    else
    {
        throw std::runtime_error("Unable to open file");
    }
}

Token Scanner::nextToken()
{
    state = 0;
    char currentChar;
    std::string content = "";
    
    
    while (true)
    {   
        if (isEOF())
        {
            return Token(TokenType::NONE, "");
        }
        currentChar = nextChar();

        switch (state)
        {
        case 0:
            if (isSpace(currentChar))
            {
                state = 0;
            }
            else if (isLetter(currentChar))
            {
                content += currentChar;
                state = 1;
            }
            else if (isdigit(currentChar))
            {
                content += currentChar;
                state = 3;
            }
            else if(currentChar=='.'){
                content +=currentChar;
                state = 4;
            }
            else if(isRelationalOperator(currentChar)){
                content+=currentChar;
                state=7;
            }
            else
            {
                throw std::runtime_error(std::string("Invalid character: ") + currentChar);
            }
            break;
        case 1:
            if (isLetter(currentChar) || isdigit(currentChar))
            {
                content += currentChar;
                state = 1;
            }
            else
            {
                state = 2;
            }
            break;
        case 2:
            back();
            return Token(TokenType::IDENTIFIER, content);




//------------------Esse espaço de case lida com os números----------------------
        case 3:
            if (isdigit(currentChar))
            {
                content += currentChar;
                state = 3;
            }
            else if(currentChar == '.'){
                
                    content += currentChar;
                    state = 4;
            
            }
            else if (isSpace(currentChar))
            {   
                state = 6;
            }
            else if (isRelationalOperator(currentChar)){
                content+=currentChar;
                back();
                state = 7;
                return Token(TokenType::NUMBER, content);
            }
            else
            {
                throw std::runtime_error("Malformed number: " + content + currentChar);
            }
            break;
        case 4:
        // Esse case serve apenas para verificar se após o '.'
            if(isDigit(currentChar)){
                
                content += currentChar;
                state = 5;
            }
            else{
                throw std::runtime_error("Malformed Float Number: " + content + currentChar);
            }
            break;
        case 5:
        // Esse case serve para verificar se só há digitos, operadores ou espaço após meu ponto flutuante
            if(isDigit(currentChar)){
                content+=currentChar;
            }
            else if(isSpace(currentChar)){
                state = 6;
            }
            else if(isRelationalOperator(currentChar)){
                state = 7;
                back();
                return Token(TokenType::NUMBER, content);
            }
            else{
                throw std::runtime_error("Malformed Float Number: " + content + currentChar);
            }
            break;
        case 6:
            back();
            return Token(TokenType::NUMBER, content);
// --------------------------------------------------------------------------------------------------
// Operadores e equações
    case 7:

        if (isRelationalOperator(currentChar)) {
            if(content.size() <= 1){
                content += currentChar;
                state = 7;
            }
            if (content.size() == 2) {
                state = 8; // Vai para o estado 8 para verificar operador de igualdade ==
            }
            
            else{
                throw std::runtime_error("Malformed Relational function: " + content + currentChar);
            }
        }
        else if (isSpace(currentChar)) {
            back();
            state = 9;
            return Token(TokenType::REL_OPERATOR, content);
        }
        else if (isdigit(currentChar)) {
            back();
            state = 3;
            return Token(TokenType::REL_OPERATOR, content);
        }
        else if (currentChar == '.') {
            back();
            state = 4;
            return Token(TokenType::REL_OPERATOR, content);
        }
        else {
            throw std::runtime_error("Malformed Relational function: " + content + currentChar);
        }
        break;

case 8:
    if (content[0] == '=' && content[1] == '=' || content[0]!='=') {
        state = 10; // Volta para o estado 7 para continuar processando operadores relacionais
    }
   
    else {
        throw std::runtime_error("Malformed Relational function: " + content);
    }
    back(); // Volta um caractere para reavaliar o operador

    break;

case 9:
        back();
        return Token(TokenType::REL_FUNCTION, content);

        
case 10:
    if(isRelationalOperator(currentChar)||isLetter(currentChar)){
        throw std::runtime_error("Malformed Relational function: " + content + currentChar);
    }
    else{
        back();
        state = 7;
    }

    break;

default:
        break;
    }


    }
}

bool Scanner::isDigit(char c){
    return (c>='0' && c<='9');
}



bool Scanner::isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Scanner::isSpace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

bool Scanner::isRelationalOperator(char c)
{
    return c == '=' || c == '>' || c == '<' || c == '!'; 
}

bool Scanner::isEquationOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

char Scanner::nextChar()
{
    return sourceBuffer[pos++];
}

void Scanner::back()
{
    pos--;
}

bool Scanner::isEOF()
{
    return pos >= sourceBuffer.size();
}
