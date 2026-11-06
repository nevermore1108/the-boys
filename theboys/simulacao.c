#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"
#include "entidades.h"
#include "auxiliares.h"
#include "eventos.h"

//Roda a simulação até chegar no fim da LEF
void simula_mundo(struct mundo *w){
    struct evento *evt_atual = malloc (sizeof (struct evento)); //Aloca memória para o evento
    int tipo, tempo;

    if (!evt_atual) //Se tiver problemas na alocação, retorna
        return;

    while (w->relogio <= T_FIM_DO_MUNDO){
        evt_atual = fprio_retira (w->lef, &tipo, &tempo);
        w->eventos_tratados++

        switch (tipo){
            case EV_CHEGA:
                evento_chega (w, evt_atual);
                break;
            case EV_ESPERA:
                evento_espera (w, evt_atual);
                break;
            case EV_DESISTE:
                evento_desiste (w, evt_atual);
                break;
            case EV_AVISA:
                evento_avisa (w, evt_atual);
                break;
            case EV_ENTRA:
                evento_entra (w, evt_atual);
                break;
            case EV_SAI:
                evento_sai (w, evt_atual);
                break;
            case EV_VIAJA:
                evento_viaja (w, evt_atual);
                break;
            case EV_MORRE:
                evento_morre (w, evt_atual);
                break;
            case EV_MISSAO:
                evento_missao (w, evt_atual);
                break;
            case EV_INVADE:
                evento_invade (w, evt_atual);
                break;
            case EV_RECUPERA_H:
                evento_recupera_h (w, evt_atual);
                break;
            case EV_RECUPERA_B:
                evento_recupera_b (w, evt_atual);
                break;
            case EV_FIM:
                evento_fim (w, evt_atual);
                break;
        }
    }

    free (evt_atual);
    return;
}