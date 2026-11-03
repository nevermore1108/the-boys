#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"
#include "entidades.h"

//Cria um herói com seus atributos
struct heroi *cria_heroi (int id){

}

//Cria uma base com seus atributos
struct base *cria_base (int id){

}

//Cria uma missão com seus atributos
struct missao *cria_missao (int id){

}

//Cria uma invasão com seus atributos
struct invasao *cria_invasao (int id){

}

//Cria o mundo, com seus heróis, bases, missões e invasões definidos
//Retorno: ponteiro para o mundo criado ou NULL se erro.
struct mundo *cria_mundo (){
    int i;
    struct mundo *w = malloc (sizeof (struct mundo)); //Aloca memória para o mundo

    if (!w) //Se tiver problemas na alocação, retorna NULL
        return NULL;

    for (i = 0; i < N_HEROIS; i++) //Cria os heróis
        w->herois[i] = cria_heroi(i);

    for (i = 0; i < N_BASES; i++) //Cria as bases
        w->bases[i] = cria_base(i);
    
    for (i = 0; i < N_MISSOES; i++) //Cria as missões
        w->missoes[i] = cria_missao(i);
    
    for (i = 0; i < N_INVASOES; i++) //Cria as invasões
        w->invasoes[i] = cria_invasao(i);
    
    w->tamanho.x = N_TAMANHO_MUNDO;
    w->tamanho.y = N_TAMANHO_MUNDO;
    w->relogio = T_INICIO;
    w->eventos_tratados = 0;
    w->lef = fprio_cria();

    return w;
}

//Roda a simulação até chegar no fim da LEF
void simulacao(struct mundo *w);

//Destrói o mundo criado, desalocando e liberando a memória utilizada 
void destroi_mundo (struct mundo *w);