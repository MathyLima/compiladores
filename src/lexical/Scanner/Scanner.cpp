#include "Scanner.h"
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <unordered_map>

// std::unordered_map<std::string, std::string>

Scanner::Scanner(const std::string &source) : pos(0), row(-1), col(0)
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
            else if (isLetter(currentChar) || currentChar == '_')
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
                if (currentChar == '!')
                {
                    content += currentChar;
                    state = 6;
                }
                else
                {
                    content += currentChar;
                    state = 5;
                }
            }

            else if (isParentesis(currentChar))
            {
                content += currentChar;
                state = 8;
            }
            else if (isEquationOperator(currentChar))
            {

                content = currentChar;
                state = 9;
            }
            else if (isHashtag(currentChar))
            {
                content += currentChar;
                state = 11;
            }
            else
            {
                throw std::runtime_error("You used a forbidden symbol at row " + std::to_string(row) + ", col " + std::to_string(col) + ": " + currentChar);
            }
            break;
        case 1:
            if (isLetter(currentChar) || isDigit(currentChar) || currentChar == '_')
            {
                content += currentChar;
                state = 1;
            }
            else if (isParentesis(currentChar))
            {
                back();
                return Token(TokenType::IDENTIFIER, content);
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

            //------------------Esse espaço de case lida com os números----------------------
        case 3:
            if (isDigit(currentChar))
            {
                content += currentChar;
                state = 3;
            }

            else if (isSpace(currentChar) || isRelationalOperator(currentChar) || isParentesis(currentChar))
            {
                back();
                return Token(TokenType::NUMBER, content);
            }
            else if (currentChar == '.')
            {
                content += currentChar;
                state = 4;
            }
            else if (isEquationOperator(currentChar))
            {
                state = 9;
                back();
                return Token(TokenType::NUMBER, content);
            }
            else
            {
                back();
                return Token(TokenType::NUMBER, content);

            }

            break;
        case 4:
            // Esse case serve apenas para verificar se após o '.'
            if (isDigit(currentChar))
            {
                content += currentChar;
                state = 4;
            }
            else if (isRelationalOperator(currentChar) || isSpace(currentChar) || isParentesis(currentChar))
            {
                back();
                return Token(TokenType::FLOAT_NUMBER, content);
            }
            else
            {
                back();
                return Token(TokenType::FLOAT_NUMBER, content);    
            }

            break;

            // --------------------------------------------------------------------------------------------------
            // Operadores e equações
        case 5:
            // Esse case serve para verificar se só há digitos, operadores ou espaço após meu ponto flutuante
            if (isRelationalOperator(currentChar) && currentChar == '=')
            {
                content += currentChar;
                state = 7;
            }
            else if (isSpace(currentChar) || isDigit(currentChar))
            {
                back();
                if (content[0] == '=')
                {
                    return Token(TokenType::EQUAL_OPERATOR, content);
                }
                else
                {
                    return Token(TokenType::REL_OPERATOR, content);
                }
            }
            else
            {   
                back();
                return Token(TokenType::LOGICAL_OPERATOR, content);
            }
            break;

        case 6:
            if (isRelationalOperator(currentChar) && currentChar == '=')
            {
                content += currentChar;
                state = 7;
            }
            else
            {
                return Token(TokenType::LOGICAL_OPERATOR, content);

            }
            break;
        case 7:
            return Token(TokenType::REL_OPERATOR, content);

            break;
            // -----------------------------------------------------------------------------------------------------------
        // Espaço para parêntesis
        case 8:
            if (isRelationalOperator(currentChar))
            {
                throw std::runtime_error(std::string("Malformed Parentesis Symbol: ") + content + currentChar);
            }
            else
            {
                if (content == "(")
                {
                    back();
                    return Token(TokenType::OPEN_PARENTESIS, content);
                }
                else if (content == ")")
                {
                    back();
                    return Token(TokenType::CLOSE_PARENTESIS, content);
                }
            }
        // Reconhece os operandos + - * /
        case 9:

            if (isDigit(currentChar))
            {
                state = 3;
                back();
                return Token(TokenType::MATH_OPERATOR, content);
            }
            else if (isSpace(currentChar))
            {
                state = 9;
                return Token(TokenType::MATH_OPERATOR, content);
            }
            else
            {
                back();
                return Token(TokenType::MATH_OPERATOR, content);
            }
            break;

        case 10:
            if (isSpace(currentChar))
            {
                state = 0;
                return Token(TokenType::EQUAL_OPERATOR, content);
            }
            else if (isDigit(currentChar))
            {
                back();
                state = 3;
                return Token(TokenType::EQUAL_OPERATOR, content);
            }
            else if (isLetter(currentChar))
            {
                back();
                state = 1;
                return Token(TokenType::EQUAL_OPERATOR, content);
            }
            else
            {
                back();
                return Token(TokenType::EQUAL_OPERATOR, content);          
            }
        case 11:
        {

            while (!isEOF() && currentChar != '\n')
            {
                currentChar = nextChar();
            }

            state = 0;
            content = "";
            back();
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
}

bool Scanner::isEOF()
{
    return pos >= sourceBuffer.size();
}
