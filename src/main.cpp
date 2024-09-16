#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"
#include "parser/Parser.h"
#include "semantico/Semantico.h"
#include <filesystem> // Para listar arquivos no diretório
#include <vector>
#include <string>

namespace fs = std::filesystem;

int main() {
    // Diretório onde estão os arquivos de teste
    std::string testDirectory = "src/arquivosTeste/benchmark-arquivos_testes";

    // Itera sobre os arquivos de teste no diretório
    for (const auto &entry : fs::directory_iterator(testDirectory)) {
        std::string filePath = entry.path().string();
        std::cout << "Testando arquivo: " << filePath << std::endl;

        std::vector<Token> tokenStream;
        Scanner sc(filePath); // Lê o arquivo atual

        Token tk;
        while (true) {
            tk = sc.nextToken();
            if (tk.getType() == TokenType::NONE) {
                break;
            }
            tokenStream.push_back({tk.getType(), tk.getText(), tk.getRow()});
            std::cout << tk.getText() << " (Type: " << static_cast<int>(tk.getType()) << ")" << std::endl;
        }

        Parser parser(tokenStream);

        try {
            ASTNode *ast = parser.parse_program(); // Gera a AST
            std::cout << "Programa Pascal analisado com sucesso!" << std::endl;

            // Gera a sequência linear de tokens
            TokenSequence tokenSequence;
            generate_token_sequence(ast, tokenSequence);

            // AnalisadorSemantico semantico;
            // semantico.processarBloco(tokenSequence.tokens);
            // tokenSequence.printSequence(); // Imprime a sequência de tokens
        } 
        catch (const SyntaxError &e) {
            std::cerr << "Erro de sintaxe no arquivo " << filePath << ": " << e.what() << std::endl;
        }
        
        std::cout << "Análise léxica do arquivo " << filePath << " foi um sucesso." << std::endl;
    }

    return 0;
}
