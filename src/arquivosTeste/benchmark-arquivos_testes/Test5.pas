program TestSemanticError;
var
   I, J: integer;
begin
   for I := 1 to 5 do
   begin
      for J := 1 to 3 do
      begin
         K := I + J;  { Erro: Variável K não foi declarada }
      end
   end;
end.
