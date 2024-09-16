program Test4;
var
   A, B, R, I : integer;

procedure teste (A:integer, B:real);
var
   S,X: real;
begin
   S := A + B * X
end;  {verifique se esse ";" no fechamento de um procedimento é necessário}

begin
   while (I <= 5) do
   begin
      A := A+1;
      B := B-1;
      R := A + B;
      I := I + 1
   end
end.

{retirar algumas palavras reservadas para gerar erros sintáticos}