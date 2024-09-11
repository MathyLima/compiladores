// #include <iostream>
// #include "lexical/Scanner/Scanner.h"
// #include "lexical/Token/Token.h"

// int main()
// {
//     Scanner sc("source_code.mc");
//     Token tk;
//     while (true)
//     {
//         tk = sc.nextToken();
//         if (tk.getType() == TokenType::NONE)
//         {
//             break;
//         }
//         std::cout << tk << std::endl;
//     }
//     std::cout << "Compilation Successful" << std::endl;

//     return 0;
// }

#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"
#include "parser/Parser.h"

int main()
{
    Scanner sc("source_code.mc");
    Token firstToken = sc.nextToken();
    std::cout << "First token: " << firstToken.getText() << std::endl;

    Parser parser(sc);
    parser.parseProgram();

    std::cout << "Compilation Successful" << std::endl;

    return 0;
}
