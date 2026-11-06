#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"
#include "entidades.h"

//Retorna um número aleatório entre min e max, inclusive
long aleat (long min, long max){
  return min + rand() % (max - min +1);
}