// programa principal do projeto "The Boys - 2026/2"
// Autor: Bernardo Fonseca do Nascimento, GRR 20264337

// seus #includes vão aqui
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "entidades.h"
#include "simulacao.h"
// seus #defines vão aqui

// minimize o uso de variáveis globais

// programa principal
int main ()
{
  srand(time (NULL));
  //Iniciar o mundo
  struct mundo *w = cria_mundo ();
  //Executar o laço de simulação
  simula_mundo(w);
  //Destruir o mundo
  w = destroi_mundo (w);

  return 0;
}

