#include <iostream>
#include "lexical/Scanner/Scanner.h"
#include "lexical/Token/Token.h"

int main()
{
    Scanner sc("source_code.mc");
    Token tk;
    while (true)
    {
        tk = sc.nextToken();
        if (tk.getType() == TokenType::NONE)
        {
            break;
        }
        std::cout << tk << std::endl;
    }
    std::cout << "Compilation Successful" << std::endl;

    return 0;
}
