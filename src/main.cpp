#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"

int main()
{
    std::vector<Token> tokenStream;
    Scanner sc("source_code.mc");

    Token tk;
    while (true)
    {
        tk = sc.nextToken();
        if (tk.getType() == TokenType::NONE)
        {
            break;
        }
        tokenStream.push_back({tk.getType(),tk.getText(),tk.getRow()});
        std::cout << tk.getText() << " (Type: " << static_cast<int>(tk.getType()) << ")" << std::endl;
    }

    // Parser parse();



    std::cout << "Analise lexica foi sucesso" << std::endl;

    return 0;
}