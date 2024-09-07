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
        {"int", "int-331"},
        {"else", "else-425"},
        {"if", "if-207"},
        {"float", "float-534"},
        {"print", "print-557"}};
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
            else if (currentChar == '.')
            {
                content += currentChar;
                state = 4;
            }
            else if (isRelationalOperator(currentChar))
            {
                content += currentChar;
                state = 7;  // Transição para operadores relacionais
            }
            else if (isParentesis(currentChar))
            {
                content += currentChar;
                state = 10; // Transição para parêntesis
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
            if (isLetter(currentChar) || isDigit(currentChar))
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
                return Token(TokenType::IDENTIFIER, existReserved->second);
            }
            else
            {
                return Token(TokenType::IDENTIFIER, content);
            }

        case 3:
            if (isDigit(currentChar)) {
                content += currentChar;
                state = 3;
            }
            else if (currentChar == '.') {
                content += currentChar;
                state = 4;
            }
            else {
                back();
                return Token(TokenType::NUMBER, content);
            }
            break;

        case 4:
            if (isDigit(currentChar)) {
                content += currentChar;
                state = 4;  // Continua processando a parte fracionária do número
            }
            else if (currentChar == 'E' || currentChar == 'e') {
                content += currentChar;
                state = 5;  // Transição para processar a parte exponencial
            }
            else {
                // Verifica se o conteúdo é um número flutuante válido
                if (content.back() == '.') {
                    throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing digits after '.'");
                }
                back();
                return Token(TokenType::FLOAT_NUMBER, content);
            }
            break;

        case 5:
            if (currentChar == '+' || currentChar == '-') {
                content += currentChar;
                state = 6;  // Transição para processar os dígitos do expoente
            }
            else if (isDigit(currentChar)) {
                content += currentChar;
                state = 6;  // Transição para processar os dígitos do expoente
            }
            else {
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 6:
            if (isDigit(currentChar)) {
                content += currentChar;
                state = 6;  // Continua processando os dígitos do expoente
            }
            else {
                // Verifica se o número exponencial é válido
                if (content.back() == 'E' || content.back() == 'e') {
                    throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing digits after exponent");
                }
                back();
                return Token(TokenType::FLOAT_NUMBER, content);
            }
            break;
        case 7:
            if (currentChar == '=') {
                content += currentChar;
                state = 9;  // Transição para verificar operador relacional com '='
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
                throw std::runtime_error(std::string("Malformed Parentesis Symbol: ") + content + currentChar);
            }
            else
            {
                if (content == "(")
                {
                    return Token(TokenType::OPEN_PARENTESIS, content);
                }
                else if (content == ")")
                {
                    return Token(TokenType::CLOSE_PARENTESIS, content);
                }
            }
            break;

        case 11:
            if (isDigit(currentChar))
            {
                back();
                return Token(TokenType::MATH_OPERATOR, content);
            }
            else
            {
                back();
                return Token(TokenType::MATH_OPERATOR, content);
            }
            break;

        case 12:
            while (!isEOF() && currentChar != '\n')
            {
                currentChar = nextChar();
            }
            state = 0;
            content = "";
            back();
            break;

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
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool Scanner::isParentesis(char c)
{
    return c == '(' || c == ')';
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
