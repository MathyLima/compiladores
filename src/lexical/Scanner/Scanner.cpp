#include "Scanner.h"
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <unordered_map>

Scanner::Scanner(const std::string &source) : pos(0), row(1), col(0)
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
    reservedWords = {
        {"program", "program-101"},
        {"var", "var-102"},
        {"integer", "integer-103"},
        {"real", "real-104"},
        {"boolean", "boolean-105"},
        {"procedure", "procedure-106"},
        {"begin", "begin-107"},
        {"end", "end-108"},
        {"if", "if-109"},
        {"then", "then-110"},
        {"else", "else-111"},
        {"while", "while-112"},
        {"do", "do-113"},
        {"not", "not-114"}};

    state = 0;
    char currentChar;
    std::string content = "";
    auto existReserved = reservedWords.find(content);

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
            else if (isDigit(currentChar))
            {
                content += currentChar;
                state = 3;
            }
            else if (isRelationalOperator(currentChar))
            {
                content += currentChar;
                state = 7;  // Transição para operadores relacionais
            }
            else if (isDelimiter(currentChar))
            {
                content+=currentChar;
                if (currentChar ==':'){
                    state = 13;
                }
                else if (currentChar == '.')
                {
                    state = 14;
                }else{
                    state = 10;
                }
            }
            else if (isEquationOperator(currentChar))
            {
                content += currentChar;
                state = 11; // Transição para operadores matemáticos
            }
            else if (isHashtag(currentChar))
            {
                content += currentChar;
                state = 12; // Transição para comentários
            }
            else
            {
                throw std::runtime_error("You used a forbidden symbol at row " + std::to_string(row) + ", col " + std::to_string(col) + ": " + currentChar);
            }
            break;

        case 1:
            if (isLetter(currentChar) || isDigit(currentChar) || currentChar=='_')
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
            existReserved = reservedWords.find(content);
            if (existReserved != reservedWords.end())
            {
                return Token(TokenType::KEYWORD, existReserved->second);
            }
            else if (content == "AND" || content == "and")
            {
                return Token(TokenType::MULT_OPERATOR,content);
            }
            else if(content == "OR" || content == "or"){
                return Token(TokenType::ADD_OPERATOR,content);
            }       
            else
            {
                return Token(TokenType::IDENTIFIER, content);
            }

        case 3:  // Parte inteira do número
            if (isDigit(currentChar)) {
                content += currentChar;
            } else if (currentChar == '.') {
                content += currentChar;
                state = 4;  // Transição para processar a parte fracionária
            } else {
                back();
                return Token(TokenType::NUMBER, content);  // Número inteiro
            }
            break;

        case 4:  // Parte fracionária ou delimitador
            if (isDigit(currentChar)||currentChar == 'E' || currentChar == 'e') {
                content += currentChar;  // Continua processando a parte fracionária
                state = 5;  // Transição para processar a parte exponencial
            } else {
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 5:  // Parte exponencial
            if (isDigit(currentChar)|| currentChar == '+' || currentChar == '-' ) {
                content += currentChar;  // Expoente sem sinal, processa diretamente
                state = 6;  // Vai para o estado que processa os dígitos do expoente
            } else {
                if(isDigit(content.back())){
                    back();
                    return Token(TokenType::FLOAT_NUMBER, content);  // Retorna número flutuante com expoente
                }
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 6:  // Processa dígitos do expoente
            if (isDigit(currentChar)) {
                content += currentChar;  // Continua processando os dígitos do expoente
            } else {
                back();
                return Token(TokenType::FLOAT_NUMBER, content);  // Retorna número flutuante com expoente
            }
            break;
        case 7:
            if (currentChar == '=') {
                content += currentChar;
                state = 9;  // Transição para verificar operador relacional com '='
            }
            else if(content.back() == '<' && currentChar == '>'){
                content += currentChar;
                return Token(TokenType::REL_OPERATOR,content);
            }
            else if (isSpace(currentChar) || isDigit(currentChar)) {
                back();
                return Token(content[0] == '=' ? TokenType::EQUAL_OPERATOR : TokenType::REL_OPERATOR, content);
            }
            else {
                back();
                return Token(TokenType::LOGICAL_OPERATOR, content);
            }
            break;

        case 8:
              if (isRelationalOperator(currentChar) && currentChar == '=')
            {
                content += currentChar;
                state = 9;
            }
            else
            {
                return Token(TokenType::LOGICAL_OPERATOR, content);
            }
            break;
        case 9:
        if(currentChar == '='){
                back();
            }
            return Token(TokenType::REL_OPERATOR, content);

            break;
        case 10:
            if (isRelationalOperator(currentChar))
            {
                back();

            }
                return Token(TokenType::DELIMITER, content);
            break;

        case 11:
            if(content.back() == '+' || content.back() == '-' ){
                back();
                return Token(TokenType::ADD_OPERATOR,content);
            }else{
                back();
                return Token(TokenType::MULT_OPERATOR,content);
            }
            break;

        case 12:
            while (!isEOF() && currentChar != '\n')
            {
                currentChar = nextChar();
            }
            state = 0;
            content = "";
            break;
        case 13:
            if(currentChar == '='){
                content+=currentChar;
                return Token(TokenType::ASSIGNMENT, content);
            }else{
                back();
                return Token(TokenType::DELIMITER,content);
            }
        case 14:
            if(isDigit(currentChar) || currentChar =='E' || currentChar == 'e'){
                content += currentChar;
                state = 4;
            }else{
                back();
                return Token(TokenType::DELIMITER,content);
            }
        default:
            break;
        }
    }
}

bool Scanner::isDigit(char c)
{
    return (c >= '0' && c <= '9');
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
    return  c == '+' || c == '-' ||  c == '*' || c == '/';
}

bool Scanner::isDelimiter(char c)
{
    return c == '(' || c == ')' || c == ';' || c == ',' || c == '.' || c == ':';
}

bool Scanner::isEquationSinal(char c)
{
    return c == '-' || c == '+';
}

bool Scanner::isHashtag(char c)
{
    return c == '#';
}

char Scanner::nextChar()
{
    char c = sourceBuffer[pos++];
    if (c == '\n')
    {
        row++;
        col = 0;
    }
    else
    {
        col++;
    }
    return c;
}

void Scanner::back()
{
    pos--;

    if (sourceBuffer[pos] == '\n') {
        row--;
        col = 0;
        size_t tempPos = pos - 1;
        while (tempPos > 0 && sourceBuffer[tempPos] != '\n') {
            col++;
            tempPos--;
        }
    } else {
        col--;
    }
}

bool Scanner::isEOF()
{
    return pos >= sourceBuffer.size();
}
