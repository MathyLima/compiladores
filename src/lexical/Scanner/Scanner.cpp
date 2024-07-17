#include "Scanner.h"
#include <fstream>
#include <stdexcept>
#include <cctype>

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
            else if (isOperator(currentChar) || isSpace(currentChar))
            {   
                state = 6;
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
            if(isDigit(currentChar)){
                content+=currentChar;
            }
            else if(isOperator(currentChar) || isSpace(currentChar)){
                state = 6;
            }
            else{
                throw std::runtime_error("Malformed Float Number: " + content + currentChar);
            }
            break;
        case 6:
            back();
            return Token(TokenType::NUMBER, content);
// --------------------------------------------------------------------------------------------------

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

bool Scanner::isOperator(char c)
{
    return c == '=' || c == '>' || c == '<' || c == '!' || c == '+' || c == '-' ||
           c == '*' || c == '/'; 
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
