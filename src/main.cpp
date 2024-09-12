#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"
#include "parser/Parser.h"

// int main()
// {
//     Scanner sc("source_code.mc");
//     Token firstToken = sc.nextToken();
//     std::cout << "First token: " << firstToken.getText() << std::endl;

//     Parser parser(sc);
//     parser.parseProgram();

//     std::cout << "Compilation Successful" << std::endl;

//     return 0;
// }

int main()
{
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
        std::cout << tk.getText() << " (Type: " << static_cast<int>(tk.getType()) << ")" << std::endl;
    }

    // Reinicialize o scanner para reiniciar a leitura do arquivo
    sc = Scanner("source_code.mc");

    // Agora, proceda para a análise sintática
    Token firstToken = sc.nextToken();
    std::cout << "First token: " << firstToken.getText() << std::endl;

    Parser parser(sc);
    parser.parseProgram();

    std::cout << "Compilation Successful" << std::endl;

    return 0;
}