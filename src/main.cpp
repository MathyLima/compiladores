#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"
#include "parser/Parser.h"
#include "semantico/Semantico.h"
#include <filesystem> // Para listar arquivos no diretório
#include <vector>
#include <string>

namespace fs = std::filesystem;

int main()
{
    // Diretório onde estão os arquivos de teste
    std::string testDirectory = "src/arquivosTeste/benchmark-arquivos_testes";
    std::string specificFileName = "Test3.pas"; // Nome do arquivo específico para teste

    // Itera sobre os arquivos de teste no diretório
    for (const auto &entry : fs::directory_iterator(testDirectory))
    {
        std::string filePath = entry.path().string();
        
        // Verifica se o arquivo é o específico que queremos testar
        if (filePath.find(specificFileName) != std::string::npos)
        {
            std::cout << "====================\n";
            std::cout << "Testando arquivo: " << filePath << std::endl;
            std::cout << "====================\n";

            std::vector<Token> tokenStream;
            Scanner sc(filePath); // Lê o arquivo atual

            Token tk;
            try
            {
                // Tentar o léxico
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

                    // Gera a sequência linear de tokens
                    TokenSequence tokenSequence;
                    generate_token_sequence(ast, tokenSequence);

                    // Chama o analisador semântico
                    AnalisadorSemantico semantico;
                    semantico.processarBloco(tokenSequence.tokens);

                    // Opcional: Imprime a sequência de tokens
                    // tokenSequence.printSequence();
                    std::cout << "Análise léxica do arquivo " << filePath << " foi um sucesso." << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Erro de sintaxe no arquivo " << filePath << ": " << e.what() << std::endl;
                }
                catch (...)
                {
                    std::cerr << "Erro desconhecido durante a análise do arquivo " << filePath << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Erro léxico no arquivo " << filePath << ": " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "Erro desconhecido durante a leitura do arquivo " << filePath << std::endl;
            }

            std::cout << "====================\n";
            break; // Para sair do loop após testar o arquivo específico
        }
    }

    return 0;
}
