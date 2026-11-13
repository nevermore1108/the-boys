#ifndef EVENTOS
#define EVENTOS
#include "entidades.h"

//Definição dos eventos
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
    int id1; //Primeiro id usado pelo evento, -1 se não existe
    int id2; //Segundo id usado pelo evento, -1 se não existe
};

//Herói H chega na base B, no instante T, e avalia se vai esperar ou não
void evento_chega (struct mundo *w, struct evento *evt);

//Herói H decide ficar na fila de espera da base B, no instante T, e avisa o porteiro
void evento_espera (struct mundo *w, struct evento *evt);

//Herói H decide não esperar e viaja para uma base aleatória D
void evento_desiste (struct mundo *w, struct evento *evt);

//Porteiro é avisado e permite ou não a entrada dos heróis na base
void evento_avisa (struct mundo *w, struct evento *evt);

//Herói H entra na base B e marca sua horá de saída
void evento_entra (struct mundo *w, struct evento *evt);

//Herói H saí da base B, avisa o porteiro e viaja para uma base D aleatória
void evento_sai (struct mundo *w, struct evento *evt);

//Herói H se desloca para uma base D, escolhida anteriormente
void evento_viaja (struct mundo *w, struct evento *evt);

//Herói H morre, no instante T. Todos seus eventos futuros são removidos da lef
void evento_morre (struct mundo *w, struct evento *evt);

//Missão M é disparada no instante T. Procura uma base para realizar, se não tiver é reagendada
void evento_missao (struct mundo *w, struct evento *evt);

/*Uma invasão I acontece na base B, no instante T. Deixa a base e os heróis inativos por um momento
|*determinado e faz todos os heróis sairem da base*/
void evento_invade (struct mundo *w, struct evento *evt);

//Após um tempo, o herói H volta a ser ativo
void evento_recupera_h (struct mundo *w, struct evento *evt);

//Após um tempo, a base B volta a ser ativa
void evento_recupera_b (struct mundo *w, struct evento *evt);

//Encerra a simulação no instante T
void evento_fim (struct mundo *w, struct evento *evt);

#endif