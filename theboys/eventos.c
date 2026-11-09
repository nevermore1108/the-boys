#include <stdio.h>
#include <stdlib.h>
#include "entidades.h"
#include "conjunto.h"
#include "fprio.h"
#include "fila.h"
#include "auxiliares.h"
#include "eventos.h"

//Herói H chega na base B, no instante T e avalia se vai esperar ou não
void evento_chega (struct mundo *w, struct evento *evt){
    int espera;
    struct evento *evt_add = malloc (sizeof (struct evento)); //Aloca memória para o evento a se adicionado

    if (!evt_add) //Se tiver problemas na alocação, retorna
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
        evt_add->tipo = EV_ESPERA; //Tipo do evento
        printf ("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA\n", evt->tempo, evt->id1, 
            evt->id2, cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);
    }
    else{ //Caso contrário, desiste
        evt_add->tipo = EV_DESISTE; //Tipo do evento
        printf ("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE\n", evt->tempo, evt->id1, 
            evt->id2, cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);
    }
        
    fprio_insere(w->lef, evt_add,evt_add->tipo,evt_add->tempo);
}

void evento_espera (struct mundo *w, struct evento *evt){
    int *heroi = malloc (sizeof(int)); //Aloca memória para o ponteiro
    *heroi = evt->id1;

    fila_insere (w->bases[evt->id2]->espera, heroi); //Insere o Herói na fila de espera

    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", evt->tempo, evt->id1, evt->id2,
        fila_tamanho(w->bases[evt->id2]->espera));

    if (fila_tamanho(w->bases[evt->id2]->espera) > w->bases[evt->id2]->max)
        w->bases[evt->id2]->max = fila_tamanho(w->bases[evt->id2]->espera);
    
    struct evento *evt_add = malloc (sizeof (struct evento)); //Aloca memória para o evento a se adicionado

    if (!evt_add) //Se tiver problemas na alocação, retorna
        return;
    
    evt_add->id1 = evt->id2; //ID1 recebe id da base
    evt_add->id2 = -1; //ID2 recebe -1;
    evt_add->tempo = evt->tempo; //Tempo recebe agora
    evt_add->tipo = EV_AVISA; //Tipo do evento
    fprio_insere(w->lef, evt_add,evt_add->tipo,evt_add->tempo);
}

void evento_desiste (struct mundo *w, struct evento *evt){
    printf("%6d: DESIST HEROI %2d BASE %d\n", evt->tempo, evt->id1, evt->id2);

    struct evento *evt_add = malloc (sizeof (struct evento)); //Aloca memória para o evento a se adicionado

    if (!evt_add) //Se tiver problemas na alocação, retorna
        return;
    
    evt_add->id1 = evt->id1; //ID1 recebe id do herói
    evt_add->id2 = aleat (0, N_BASES); //ID2 recebe uma base destino aleatória;
    evt_add->tempo = evt->tempo; //Tempo recebe agora
    evt_add->tipo = EV_VIAJA; //Tipo do evento
    fprio_insere(w->lef, evt_add,evt_add->tipo,evt_add->tempo);
}

void evento_avisa (struct mundo *w, struct evento *evt){
    int *heroi;

    printf("%6d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [ ", evt->tempo, evt->id1, 
        cjto_card (w->bases[evt->id1]->presentes), w->bases[evt->id1]->lotacao);
    fila_imprime(w->bases[evt->id1]->espera);
    printf(" ]\n");

    while ((w->bases[evt->id1]->lotacao > cjto_card (w->bases[evt->id1]->presentes))
    && (fila_tamanho (w->bases[evt->id1]->espera) > 0)){
        heroi = fila_retira (w->bases[evt->id1]->espera);
        cjto_insere (w->bases[evt->id1]->presentes, *heroi);

        struct evento *evt_add = malloc (sizeof (struct evento)); //Aloca memória para o evento a se adicionado

        if (!evt_add) //Se tiver problemas na alocação, retorna
            return;
    
        evt_add->id1 = *heroi; //ID1 recebe id do herói
        evt_add->id2 = evt->id1; //ID2 recebe o id da base;
        evt_add->tempo = evt->tempo; //Tempo recebe agora
        evt_add->tipo = EV_ENTRA; //Tipo do evento
        fprio_insere(w->lef, evt_add,evt_add->tipo,evt_add->tempo);

        free(heroi);
    }
}

void evento_fim (struct mundo *w, struct evento *evt){
    int i, min_missoes, max_missoes;
    int mortos = 0, missoes_cumpr = 0, soma = 0;
    double media, taxa_missoes_cumpr, taxa_invasoes, taxa_mort;
    w->relogio = T_FIM_DO_MUNDO + 1; //Seta o relógio para além do final

    printf ("%6d: FIM\n", evt->tempo);

    for (i = 0; i < N_HEROIS; i++){ //Percorre os heróis contando os mortos
        if (w->herois[i]->morto){
            mortos++;
            printf("HEROI %2d MORTO PAC %3d VEL %4d EXP %4d HABS [ ", i, w->herois[i]->paciencia,
            w->herois[i]->vel, w->herois[i]->exp);
            cjto_imprime (w->herois[i]->hab);
            printf(" ]\n");
        }
        else{
            printf("HEROI %2d VIVO PAC %3d VEL %4d EXP %4d HABS [ ", i, w->herois[i]->paciencia,
            w->herois[i]->vel, w->herois[i]->exp);
            cjto_imprime (w->herois[i]->hab);
            printf(" ]\n");
        }
    }

    for (i = 0; i < N_BASES; i++){ //Percorre as bases contando as missoes cumpridas
        missoes_cumpr += w->bases[i]->missoes;
        printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", i, w->bases[i]->lotacao, 
        w->bases[i]->max, w->bases[i]->missoes);
    }
    
    printf("EVENTOS TRATADOS: %d\n", w->eventos_tratados);

    taxa_missoes_cumpr = ((missoes_cumpr/N_MISSOES)*100);
    printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", missoes_cumpr, N_MISSOES, taxa_missoes_cumpr);

    taxa_invasoes = ((w->invasoes_realizadas/N_INVASOES)*100);
    printf("INVASÕES REALIZADAS: %d/%d (%.1f%%)\n", w->invasoes_realizadas, N_INVASOES, taxa_invasoes);

    //Inicializações para os cálculos das missões
    min_missoes = w->missoes[0]->tentativas;
    max_missoes = w->missoes[0]->tentativas;
    soma += w->missoes[0]->tentativas;

    for (i = 1; i < N_MISSOES; i++){ //Calcula a menor, a maior e a soma das tentativas feitas nas missões
        soma += w->missoes[i]->tentativas;
        if (w->missoes[i]->tentativas < min_missoes)
            min_missoes = w->missoes[i]->tentativas;
        if (w->missoes[i]->tentativas > max_missoes)
            max_missoes = w->missoes[i]->tentativas; 
    }
    media = soma/N_MISSOES; //Calcula a média de tentativas

    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", min_missoes, max_missoes, media);

    taxa_mort = ((mortos/N_HEROIS)*100);
    printf("TAXA MORTALIDADE: %.1f%%\n", taxa_mort);
}