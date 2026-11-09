#include <stdio.h>
#include "entidades.h"
#include "conjunto.h"
#include "fprio.h"
#include "fila.h"
#include "eventos.h"

//Herói H chega na base B, no instante T e avalia se vai esperar ou não
void evento_chega (struct mundo *w, struct evento *evt){
    int espera;
    struct evento *evt_add = malloc (sizeof (struct evento)); //Aloca memória para o evento a se adicionado

    if (!evt) //Se tiver problemas na alocação, retorna
        return;

    w->herois[evt->id1]->base = evt->id2; //Altera a base do herói para a base que chegou

    //Vê se a base tem vaga e se a fila de espera está vazia
    if ((cjto_card(w->bases[evt->id2]->presentes) != w->bases[evt->id2]->lotacao)
        && (fila_tamanho(w->bases[evt->id2]->espera) == 0))
        espera = 1; //Se sim, espera é verdade
    else //Se não, depende da paciência do herói
        espera = (w->herois[evt->id1]->paciencia > (10*fila_tamanho(w->bases[evt->id2]->espera)));
    
    //Prepara os ids e tempo do próximo evento (iguais independente do tipo NESSE caso)
    evt_add->id1 = evt->id1; //ID1 recebe id do herói
    evt_add->id2 = evt->id2; //ID2 recebe id da base
    evt_add->tempo = evt->tempo; //Tempo recebe agora

    //Se espera for verdade e a base estiver ativa, o herói espera
    if ((espera) && (w->bases[evt->id2]->estado)){
        evt->tipo = EV_ESPERA; //Tipo do evento
        printf ("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA\n", evt->tempo, evt->id1, 
            evt->id2, cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);
    }
    else{ //Caso contrário, desiste
        evt->tipo = EV_DESISTE; //Tipo do evento
        printf ("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE\n", evt->tempo, evt->id1, 
            evt->id2, cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);
    }
        
    fprio_insere(w->lef, evt_add,evt_add->tipo,evt_add->tempo);
}