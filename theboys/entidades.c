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
//Retorno: NULL
struct mundo *destroi_mundo (struct mundo *w){
    int i;
    if (!w) //Se não existir mundo, retorna
        return;

    for (i = 0; i < N_HEROIS; i++){ //Libera os heróis
        if (w->herois[i]){
            if (w->herois[i]->hab) //Destroi o conjunto de habilidades
                w->herois[i]->hab = cjto_destroi(w->herois[i]->hab);

            free(w->herois[i]);
        }
    }
    free (w->herois);

    for (i = 0; i < N_BASES; i++){ //Libera as bases
        if (w->bases[i]){
            if (w->bases[i]->presentes) //Destroi o conjunto de presentes
                w->bases[i]->presentes = cjto_destroi(w->bases[i]->presentes);

            if (w->bases[i]->espera) //Destroi a fila de espera
                w->bases[i]->espera = fila_destroi(w->bases[i]->espera);

            free(w->bases[i]);
        }
    }
    free (w->bases);

    for (i = 0; i < N_MISSOES; i++){ //Libera as missões
        if (w->missoes[i]){
            if (w->missoes[i]->hab) //Destroi o conjunto de habilidades
                w->missoes[i]->hab = cjto_destroi(w->missoes[i]->hab);

            free(w->missoes[i]);
        }
    }
    free (w->missoes);

    for (i = 0; i < N_INVASOES; i++){ //Libera as invasões
        if (w->invasoes[i])
            free(w->invasoes[i]);
    }
    free (w->invasoes);

    w->lef = fprio_destroi(w->lef);
    free(w);

    return NULL;
}