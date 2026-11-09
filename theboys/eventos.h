#ifndef EVENTOS
#define EVENTOS
#include "entidades.h"

#define EV_CHEGA 1
#define EV_ESPERA 2
#define EV_DESISTE 3
#define EV_AVISA 4
#define EV_ENTRA 5
#define EV_SAI 6
#define EV_VIAJA 7
#define EV_MORRE 8
#define EV_MISSAO 9
#define EV_INVADE 10
#define EV_RECUPERA_H 11
#define EV_RECUPERA_B 12
#define EV_FIM 13

struct evento{
    int tipo; //Tipo do evento 1 a 12
    int tempo; //Momento que o evento acontece
    int id1; //Primeiro id usado pelo evento
    int id2; //Segundo id usado pelo evento, NULL se não existe
};

//Herói H chega na base B, no instante T e avalia se vai esperar ou não
void evento_chega (struct mundo *w, struct evento *evt);

void evento_espera (struct mundo *w, struct evento *evt);

void evento_desiste (struct mundo *w, struct evento *evt);

void evento_avisa (struct mundo *w, struct evento *evt);

void evento_entra (struct mundo *w, struct evento *evt);

void evento_sai (struct mundo *w, struct evento *evt);

void evento_viaja (struct mundo *w, struct evento *evt);

void evento_morre (struct mundo *w, struct evento *evt);

void evento_missao (struct mundo *w, struct evento *evt);

void evento_invade (struct mundo *w, struct evento *evt);

void evento_recupera_h (struct mundo *w, struct evento *evt);

void evento_recupera_b (struct mundo *w, struct evento *evt);

void evento_fim (struct mundo *w, struct evento *evt);

#endif