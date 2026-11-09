#include <stdlib.h>

//Retorna um número aleatório entre min e max, inclusive
long aleat (long min, long max){
  return min + rand() % (max - min +1);
}