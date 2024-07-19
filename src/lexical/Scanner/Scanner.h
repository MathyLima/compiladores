#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <unordered_map>

#include "../Token/Token.h"

class Scanner
{
private:
    int state;
    std::vector<char> sourceBuffer;
    int pos;
    int row;
    int col;
    std::unordered_map<std::string, std::string> reservedWords;

public:
    Scanner(const std::string &source);
    Token nextToken();

private:
    bool isDigit(char c);
    bool isFloatPoint(char c);
    bool isLetter(char c);
    bool isSpace(char c);
    bool isRelationalOperator(char c);
    bool isEquationOperator(char c);
    bool isEquationSinal(char c);
    bool isParentesis(char c);
    char nextChar();
    void back();
    bool isEOF();
};

#endif
