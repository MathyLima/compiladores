program test1;		{Este arquivo representa um programa correto}

var			{Existem diversos erros que podem ser gerados neste arquivo. Alguns exemplos:}
 
  a,b,c: integer;      	
   
  x,y,z: real;		{Declare novamente a vari�vel "a" como real para que gere um erro "vari�vel j� declarada"}
  
  chave: boolean;

  procedure LimparTela;	
   
  begin			
   
  end;



  procedure Somatorio(entrada:integer);
   
  var
      
     resultado: integer; {Declare a vari�vel "a" novamente. Neste caso n�o deve gerar erro poiso escopo � outro}
  
  begin

     resultado := 0;
      
     {LimparTela; adicione este procedimento}
     while (entrada>0) do {no lugar de "entrada" use "input" e veja se gera o erro "vari�vel n�o declarada" }
                         resultado := resultado + entrada;
     entrada := entrada - 1		{troque "1" por "chave" e veja se gera o erro "tipos incompat�veis"}	
     
  end;




begin
   
   LimparTela;
   
   Somatorio(a);
   x:= y + z * (5.5 - c) / 2.567; {troque "x" por "a" e veja se gera o erro "tipos incompat�veis"}
  
   if chave then 

      if x <> z then {substitua x por chave} 
 
         z := 5.0 {troque "5.0" por "test1" e veja se gera erro "Nome do programa n�o pode ser usado"}
  
      else
   
      chave := not chave

end. 