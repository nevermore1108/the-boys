#include <stdlib.h>
#include <math.h>
#include "entidades.h"

//Retorna um número aleatório entre min e max, inclusive
long aleat (long min, long max){
  return min + rand() % (max - min +1);
}

double distancia (struct posicao a, struct posicao b){
  return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}