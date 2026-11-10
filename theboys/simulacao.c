#include <stdlib.h>
#include "fprio.h"
#include "entidades.h"
#include "eventos.h"

//Roda a simulação até chegar no fim da LEF
void simula_mundo(struct mundo *w){
    struct evento *evt_atual;
    int tipo, tempo;

    while (w->relogio <= T_FIM_DO_MUNDO){ //Enquanto o relógio não for maior, simula o mundo
        evt_atual = fprio_retira (w->lef, &tipo, &tempo); //Remove o evento atual da lef
        w->eventos_tratados++; //Aumenta a quantidade de eventos tratados

        switch (tipo){ //Chama os eventos
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
            /*case EV_SAI:
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
                break;*/
            case EV_FIM:
                evento_fim (w, evt_atual);
                break;
        }
        free (evt_atual);
    }
    return;
}