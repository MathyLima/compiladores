#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"
#include "parser/Parser.h"

int main()
{
    std::vector<Token> tokenStream;
    Scanner sc("source_code.mc");

    // Primeiro, imprima todos os tokens gerados pelo scanner
    std::cout << "Token sequence:" << std::endl;
    Token tk;
    while (true)
    {
        tk = sc.nextToken();
        if (tk.getType() == TokenType::NONE)
        {
            break;
        }
        tokenStream.push_back(tk);
        std::cout << tk.getText() << " (Type: " << static_cast<int>(tk.getType()) << ")" << std::endl;
    

    }
    //analisadorSintatico(tokenStream);

    std::cout << "Analise lexica foi sucesso" << std::endl;

    return 0;
}