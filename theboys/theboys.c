// programa principal do projeto "The Boys - 2026/2"
// Autor: Bernardo Fonseca do Nascimento, GRR 20264337

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "entidades.h"
#include "simulacao.h"

int main (){
  srand(time (NULL));

  struct mundo *w = cria_mundo ();

  simula_mundo(w);

  w = destroi_mundo (w);

  return 0;
}

