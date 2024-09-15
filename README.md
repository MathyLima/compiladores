# Git Para os projetos da disciplina de compiladores

## Compile:

g++ src/main.cpp src/lexical/Scanner/Scanner.cpp src/lexical/Token/Token.cpp -o compiler

## [Caio] novo comando para rodar, agora com o Parser.cpp
g++ src/main.cpp src/lexical/Scanner/Scanner.cpp src/parser/Parser.cpp -o compiler


## Execute

./compiler
