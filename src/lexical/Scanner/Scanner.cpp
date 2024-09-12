#include "Scanner.h"
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <unordered_map>

bool analisadorSintatico(const Token &token) {
    std::cout << "chamei o analisador sintatico pra " << token << std::endl;
    return true;
};

bool verificarSintaxe(const Token &token, int row, int col)
{
    if (!analisadorSintatico(token))
    {
        std::cerr << "syntax error na linha " << row << " e coluna " << col << std::endl;
        return false;
    }
    return true;
}
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
        {"program", "program"},
        {"var", "var"},
        {"integer", "integer"},
        {"real", "real"},
        {"boolean", "boolean"},
        {"procedure", "procedure"},
        {"begin", "begin"},
        {"end", "end"},
        {"if", "if"},
        {"then", "then"},
        {"else", "else"},
        {"while", "while"},
        {"do", "do"},
        {"not", "not"}};

    state = 0;
    char currentChar;
    std::string content = "";
    auto existReserved = reservedWords.find(content);

    while (true)
    {
        if (isEOF())
        {
            currentToken = Token(TokenType::NONE, "");
            return currentToken;
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
                state = 7;
            }
            else if (isDelimiter(currentChar))
            {
                content += currentChar;
                if (currentChar == ':')
                {
                    state = 13;
                }
                else if (currentChar == '.')
                {
                    state = 14;
                }
                else
                {
                    state = 10;
                }
            }
            else if (isEquationOperator(currentChar))
            {
                content += currentChar;
                state = 11; 
            }
            else if (isHashtag(currentChar))
            {
                content += currentChar;
                state = 12; 
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
            else
            {
                
                back();
                back();
                currentChar = nextChar();
                state = 2;
            }
            break;

        case 2:
            existReserved = reservedWords.find(content);
            if (existReserved != reservedWords.end())
            {
                currentToken = Token(TokenType::KEYWORD, existReserved->second);
            }
            else if (content == "AND" || content == "and")
            {
                currentToken = Token(TokenType::MULT_OPERATOR, content);
            }
            else if (content == "OR" || content == "or")
            {
                currentToken = Token(TokenType::ADD_OPERATOR, content);
            }
            else
            {
                currentToken = Token(TokenType::IDENTIFIER, content);
            }
            
            
            back();
            verificarSintaxe(currentToken, row, col);
            return currentToken;

        case 3: 
            if (isDigit(currentChar))
            {
                content += currentChar;
            }
            else if (currentChar == '.')
            {
                content += currentChar;
                state = 4;
            }
            else
            {
                back();
                currentToken = Token(TokenType::NUMBER, content);
              
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            break;

        case 4: 
            if (isDigit(currentChar) || currentChar == 'E' || currentChar == 'e')
            {
                content += currentChar; 
                state = 5;              
            }
            else
            {
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 5: 
            if (isDigit(currentChar) || currentChar == '+' || currentChar == '-')
            {
                content += currentChar;
                state = 6;             
            }
            else
            {
                if (isDigit(content.back()))
                {
                    back();
                    currentToken = Token(TokenType::FLOAT_NUMBER, content);
                    
                     verificarSintaxe(currentToken, row, col);
                    return currentToken; 
                }
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 6:
            if (isDigit(currentChar))
            {
                content += currentChar; 
            }
            else
            {
                back();
                currentToken = Token(TokenType::FLOAT_NUMBER, content);
               
                 verificarSintaxe(currentToken, row, col);
                return currentToken; 
            }
            break;
        case 7:
            if (currentChar == '=')
            {
                content += currentChar;
                state = 9; 
            }
            else if (content.back() == '<' && currentChar == '>')
            {
                content += currentChar;
                currentToken = Token(TokenType::REL_OPERATOR, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else if (isSpace(currentChar) || isDigit(currentChar))
            {
                back();
                currentToken = Token(content[0] == '=' ? TokenType::EQUAL_OPERATOR : TokenType::REL_OPERATOR, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else
            {
                back();
                currentToken = Token(TokenType::REL_OPERATOR, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
        case 8:
            if (isRelationalOperator(currentChar) && currentChar == '=')
            {
                content += currentChar;
                state = 9;
            }
            else
            {
                currentToken = Token(TokenType::LOGICAL_OPERATOR, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            break;
        case 9:
            if (currentChar == '=')
            {
                back();
            }
            currentToken = Token(TokenType::REL_OPERATOR, content);
            return currentToken;

            break;
        case 10:
            if (currentChar == ';')
            {
                currentToken = Token(TokenType::SEMICOLON, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else if (currentChar == ',')
            {
                currentToken = Token(TokenType::COMMA, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else if (currentChar == '.')
            {
                currentToken = Token(TokenType::DOT, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else if (currentChar == ':')
            {
                currentToken = Token(TokenType::COLON, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else if (currentChar == '(')
            {
                currentToken = Token(TokenType::LPAREN, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else if (currentChar == ')')
            {
                currentToken = Token(TokenType::RPAREN, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else
            {
                currentToken = Token(TokenType::DELIMITER, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }

        case 11:
            if (content.back() == '+' || content.back() == '-')
            {
                back();
                currentToken = Token(TokenType::ADD_OPERATOR, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else
            {
                back();
                currentToken = Token(TokenType::MULT_OPERATOR, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
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
            if (currentChar == '=')
            {
                content += currentChar;
                currentToken = Token(TokenType::ASSIGNMENT, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
            else
            {
                back();
                currentToken = Token(TokenType::DELIMITER, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
        case 14:
            if (isDigit(currentChar) || currentChar == 'E' || currentChar == 'e')
            {
                content += currentChar;
                state = 4;
            }
            else
            {
                back();
                currentToken = Token(TokenType::DELIMITER, content);
                 verificarSintaxe(currentToken, row, col);
                return currentToken;
            }
        default:
            break;
        }
    }
}

Token Scanner::getCurrentToken()
{
    return currentToken; 
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

    if (sourceBuffer[pos] == '\n')
    {
        row--;
        col = 0;
        size_t tempPos = pos - 1;
        while (tempPos > 0 && sourceBuffer[tempPos] != '\n')
        {
            col++;
            tempPos--;
        }
    }
    else
    {
        col--;
    }
}

bool Scanner::isEOF()
{
    return pos >= sourceBuffer.size();
}
