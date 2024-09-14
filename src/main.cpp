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

        NodeLevel nodeLevels;
        build_node_levels(ast, nodeLevels); // Constrói o array de arrays
        std::cout << "\nConteúdo do array de arrays:" << std::endl;
        nodeLevels.printLevels(); // Imprime os níveis de nós
    }
    catch (const SyntaxError &e)
    {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Analise lexica foi sucesso" << std::endl;

    return 0;
}