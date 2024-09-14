#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"
#include "parser/Parser.h"

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
        tokenStream.push_back({tk.getType(), tk.getText(), tk.getRow()});
        std::cout << tk.getText() << " (Type: " << static_cast<int>(tk.getType()) << ")" << std::endl;
    }

    Parser parser(tokenStream);

    try
    {
        ASTNode *ast = parser.parse_program(); // Gera a AST
        std::cout << "Programa Pascal analisado com sucesso!" << std::endl;

        // Alteração para utilizar a nova função generate_token_sequence
        TokenSequence tokenSequence;
        generate_token_sequence(ast, tokenSequence); // Gera a sequência linear de tokens

        std::cout << "\nConteúdo da sequência de tokens:" << std::endl;
        tokenSequence.printSequence(); // Imprime a sequência de tokens
    }
    catch (const SyntaxError &e)
    {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Análise léxica foi um sucesso" << std::endl;

    return 0;
}
