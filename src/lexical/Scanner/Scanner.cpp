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
        {"not", "not"},
        {"for", "for"},
        {"to", "to"}};
}

Token Scanner::nextToken()
{
   

    state = 0;
    char currentChar;
    std::string content = "";
    auto existReserved = reservedWords.find(content);

    while (true)
    {
        if (isEOF())
        {
            if( content == ".") {
                currentToken = Token(TokenType::DELIMITER, content, std::to_string(row));
                return currentToken;
            }
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
                state = 7; // Transição para operadores relacionais
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
                else if (currentChar == '{')
                {
                    state = 15; // Novo estado para ignorar comentários entre {}
                }
                else
                {
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
            if (isLetter(currentChar) || isDigit(currentChar) || currentChar == '_')
            {
                content += currentChar;
            }
            else
            {
                back();
                back();
                state = 2;
            }
            break;

        case 2:
            existReserved = reservedWords.find(content);
            if (existReserved != reservedWords.end())
            {
                currentToken = Token(TokenType::KEYWORD, existReserved->second, std::to_string(row));
            }
            else if (content == "AND" || content == "and")
            {
                currentToken = Token(TokenType::AND, content, std::to_string(row));
            }
            else if (content == "OR" || content == "or")
            {
                currentToken = Token(TokenType::OR, content, std::to_string(row));
            }
            else if (content == "FOR" || content == "for")
            {
                currentToken = Token(TokenType::FOR, content, std::to_string(row));
            }
            else if (content == "TO" || content == "to")
            {
                currentToken = Token(TokenType::TO, content, std::to_string(row));
            }
            else
            {
                currentToken = Token(TokenType::IDENTIFIER, content, std::to_string(row));
            }
            return currentToken;

        case 3: // Parte inteira do número
            if (isDigit(currentChar))
            {
                content += currentChar;
            }
            else if (currentChar == '.')
            {
                content += currentChar;
                state = 4; // Transição para processar a parte fracionária
            }
            else
            {
                back();
                currentToken = Token(TokenType::NUMBER, content, std::to_string(row));
                return currentToken; // Número inteiro
            }
            break;

        case 4: // Parte fracionária ou delimitador
            if (isDigit(currentChar) || currentChar == 'E' || currentChar == 'e')
            {
                content += currentChar; // Continua processando a parte fracionária
                state = 5;              // Transição para processar a parte exponencial
            }
            else
            {
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 5: // Parte exponencial
            if (isDigit(currentChar) || currentChar == '+' || currentChar == '-')
            {
                content += currentChar; // Expoente sem sinal, processa diretamente
                state = 6;              // Vai para o estado que processa os dígitos do expoente
            }
            else
            {
                if (isDigit(content.back()))
                {
                    back();
                    currentToken = Token(TokenType::FLOAT_NUMBER, content, std::to_string(row));
                    return currentToken; // Retorna número flutuante com expoente
                }
                throw std::runtime_error("Malformed floating point number at row " + std::to_string(row) + ", col " + std::to_string(col) + ": missing exponent digits");
            }
            break;

        case 6: // Processa dígitos do expoente
            if (isDigit(currentChar))
            {
                content += currentChar; // Continua processando os dígitos do expoente
            }
            else
            {
                back();
                currentToken = Token(TokenType::FLOAT_NUMBER, content, std::to_string(row));
                return currentToken; // Retorna número flutuante com expoente
            }
            break;
        case 7:
            if (currentChar == '=')
            {
                content += currentChar;
                state = 9; // Transição para verificar operador relacional com '='
            }
            else if (content.back() == '<' && currentChar == '>')
            {
                content += currentChar;
                currentToken = Token(TokenType::REL_OPERATOR, content, std::to_string(row));
                return currentToken;
            }
            else if (isSpace(currentChar) || isDigit(currentChar))
            {
                back();
                currentToken = Token(content[0] == '=' ? TokenType::EQUAL_OPERATOR : TokenType::REL_OPERATOR, content, std::to_string(row));
                return currentToken;
            }
            else
            {
                back();
                currentToken = Token(TokenType::LOGICAL_OPERATOR, content, std::to_string(row));
                return currentToken;
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
                currentToken = Token(TokenType::LOGICAL_OPERATOR, content, std::to_string(row));
                return currentToken;
            }
            break;
        case 9:
            if (currentChar == '=')
            {
                back();
            }
            currentToken = Token(TokenType::REL_OPERATOR, content, std::to_string(row));
            return currentToken;

            break;
        case 10:
             back(); // Retrocede para reprocessar o caractere atual
            currentToken = Token(TokenType::DELIMITER, content, std::to_string(row));
            return currentToken;

        case 11:
            if (content.back() == '+' || content.back() == '-')
            {
                back();
                currentToken = Token(TokenType::ADD_OPERATOR, content, std::to_string(row));
                return currentToken;
            }
            else
            {
                back();
                currentToken = Token(TokenType::MULT_OPERATOR, content, std::to_string(row));
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
                currentToken = Token(TokenType::ASSIGNMENT, content, std::to_string(row));
                return currentToken;
            }
            else
            {
                back();
                currentToken = Token(TokenType::DELIMITER, content, std::to_string(row));
                return currentToken;
            }
        case 14:
            if(isDigit(currentChar) || currentChar == 'E' || currentChar == 'e')
            {
                content += currentChar;
                state = 4;
            }
            else
            {
                // Tratamento padrão para o ponto final (.)
                currentToken = Token(TokenType::DELIMITER, content, std::to_string(row));
                return currentToken;
            }
        case 15: // Ignorar tudo entre { e }
            while (!isEOF() && currentChar != '}')
            {
                currentChar = nextChar();
            }
            if (currentChar == '}')
            {
                state = 0;    // Voltar ao estado inicial
                content = ""; // Reseta o conteúdo já que é um comentário
            }
            else
            {
                throw std::runtime_error("Unterminated comment at row " + std::to_string(row) + ", col " + std::to_string(col));
            }
            break;

        default:
            break;
        }
    }
}

Token Scanner::getCurrentToken()
{
    return currentToken; // Apenas retorna o token atual sem avançar
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
    return c == '(' || c == ')' || c == ';' || c == ',' || c == '.' || c == ':' || c =='{' || c == '}';
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
    if (isEOF())
    {
        return '\0'; // Retorna caractere nulo ou um valor especial para EOF
    }

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
    if (pos > 0)
    {
        pos--;
        char c = sourceBuffer[pos];
        if (c == '\n')
        {
            row--;
            // Recalcular 'col' se necessário
        }
        else
        {
            col--;
        }
    }
}

bool Scanner::isEOF()
{
    return pos >= sourceBuffer.size();
}