#include <stdio.h>
#include <stdlib.h>
#include "entidades.h"
#include "conjunto.h"
#include "fprio.h"
#include "fila.h"
#include "auxiliares.h"
#include "eventos.h"

// Herói H chega na base B, no instante T e avalia se vai esperar ou não
void evento_chega(struct mundo *w, struct evento *evt)
{
    int espera;
    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add) // Se tiver problemas na alocação, retorna
        return;

    w->herois[evt->id1]->base = evt->id2; // Altera a base do herói para a base que chegou

    // Vê se a base tem vaga e se a fila de espera está vazia
    if ((cjto_card(w->bases[evt->id2]->presentes) != w->bases[evt->id2]->lotacao) && (fila_tamanho(w->bases[evt->id2]->espera) == 0))
        espera = 1; // Se sim, espera é verdade
    else            // Se não, depende da paciência do herói
        espera = (w->herois[evt->id1]->paciencia > (10 * fila_tamanho(w->bases[evt->id2]->espera)));

    // Prepara os ids e tempo do próximo evento (iguais independente do tipo NESSE caso)
    evt_add->id1 = evt->id1;     // ID1 recebe id do herói
    evt_add->id2 = evt->id2;     // ID2 recebe id da base
    evt_add->tempo = evt->tempo; // Tempo recebe agora

    // Se espera for verdade e a base estiver ativa, o herói espera
    if ((espera) && (w->bases[evt->id2]->estado))
    {
        evt_add->tipo = EV_ESPERA; // Tipo do evento
        printf("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) ESPERA\n", evt->tempo, evt->id1,
               evt->id2, cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);
    }
    else
    {                               // Caso contrário, desiste
        evt_add->tipo = EV_DESISTE; // Tipo do evento
        printf("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) DESISTE\n", evt->tempo, evt->id1,
               evt->id2, cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);
    }

    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
}

void evento_espera(struct mundo *w, struct evento *evt)
{
    int *heroi = malloc(sizeof(int)); // Aloca memória para o ponteiro
    *heroi = evt->id1;

    fila_insere(w->bases[evt->id2]->espera, heroi); // Insere o Herói na fila de espera

    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", evt->tempo, evt->id1, evt->id2,
           fila_tamanho(w->bases[evt->id2]->espera));

    if (fila_tamanho(w->bases[evt->id2]->espera) > w->bases[evt->id2]->max)
        w->bases[evt->id2]->max = fila_tamanho(w->bases[evt->id2]->espera);

    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add) // Se tiver problemas na alocação, retorna
        return;

    evt_add->id1 = evt->id2;     // ID1 recebe id da base
    evt_add->id2 = -1;           // ID2 recebe -1;
    evt_add->tempo = evt->tempo; // Tempo recebe agora
    evt_add->tipo = EV_AVISA;    // Tipo do evento
    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
}

void evento_desiste(struct mundo *w, struct evento *evt)
{
    printf("%6d: DESIST HEROI %2d BASE %d\n", evt->tempo, evt->id1, evt->id2);

    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add) // Se tiver problemas na alocação, retorna
        return;

    evt_add->id1 = evt->id1;              // ID1 recebe id do herói
    evt_add->id2 = aleat(0, N_BASES - 1); // ID2 recebe uma base destino aleatória;
    evt_add->tempo = evt->tempo;          // Tempo recebe agora
    evt_add->tipo = EV_VIAJA;             // Tipo do evento
    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
}

void evento_avisa(struct mundo *w, struct evento *evt)
{
    printf("%6d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [ ", evt->tempo, evt->id1,
           cjto_card(w->bases[evt->id1]->presentes), w->bases[evt->id1]->lotacao);
    fila_imprime(w->bases[evt->id1]->espera);
    printf(" ]\n");

    while ((w->bases[evt->id1]->lotacao > cjto_card(w->bases[evt->id1]->presentes)) && (fila_tamanho(w->bases[evt->id1]->espera) != 0))
    {
        int *prim = fila_retira(w->bases[evt->id1]->espera);
        int id_heroi = *prim;
        free(prim);

        cjto_insere(w->bases[evt->id1]->presentes, id_heroi);
        printf("%6d: AVISA PORTEIRO BASE %d ADMITE %2d\n", evt->tempo, evt->id1, id_heroi);

        struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

        if (!evt_add) // Se tiver problemas na alocação, retorna
            return;

        evt_add->id1 = id_heroi;     // ID1 recebe id do herói
        evt_add->id2 = evt->id1;     // ID2 recebe o id da base;
        evt_add->tempo = evt->tempo; // Tempo recebe agora
        evt_add->tipo = EV_ENTRA;    // Tipo do evento
        fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
    }
}

void evento_entra(struct mundo *w, struct evento *evt)
{
    int tpb = 15 + (w->herois[evt->id1]->paciencia * aleat(1, 20));

    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add) // Se tiver problemas na alocação, retorna
        return;

    evt_add->id1 = evt->id1;             // ID1 recebe id do herói
    evt_add->id2 = evt->id2;             // ID2 recebe o id da base;
    evt_add->tempo = (evt->tempo) + tpb; // Tempo recebe agora
    evt_add->tipo = EV_SAI;              // Tipo do evento
    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);

    printf("%6d: ENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d\n", evt->tempo, evt->id1, evt->id2,
           cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao, evt_add->tempo);
}

void evento_sai(struct mundo *w, struct evento *evt)
{
    cjto_retira(w->bases[evt->id2]->presentes, evt->id1);

    printf("%6d: SAI HEROI %2d BASE %d (%2d/%2d)\n", evt->tempo, evt->id1, evt->id2,
           cjto_card(w->bases[evt->id2]->presentes), w->bases[evt->id2]->lotacao);

    struct evento *evt_add1 = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add1) // Se tiver problemas na alocação, retorna
        return;

    evt_add1->id1 = evt->id1;              // ID1 recebe id do herói
    evt_add1->id2 = aleat(0, N_BASES - 1); // ID2 recebe uma base destino aleatória;
    evt_add1->tempo = evt->tempo;          // Tempo recebe agora
    evt_add1->tipo = EV_VIAJA;             // Tipo do evento
    fprio_insere(w->lef, evt_add1, evt_add1->tipo, evt_add1->tempo);

    struct evento *evt_add2 = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add2) // Se tiver problemas na alocação, retorna
        return;

    evt_add2->id1 = evt->id2;     // ID1 recebe id da base
    evt_add2->id2 = -1;           // ID2 recebe -1;
    evt_add2->tempo = evt->tempo; // Tempo recebe agora
    evt_add2->tipo = EV_AVISA;    // Tipo do evento
    fprio_insere(w->lef, evt_add2, evt_add2->tipo, evt_add2->tempo);
}

void evento_viaja(struct mundo *w, struct evento *evt)
{
    int dist = distancia(w->bases[w->herois[evt->id1]->base]->local, w->bases[evt->id2]->local);
    int duracao = dist / (w->herois[evt->id1]->vel);

    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add) // Se tiver problemas na alocação, retorna
        return;

    evt_add->id1 = evt->id1;                 // ID1 recebe id do herói
    evt_add->id2 = evt->id2;                 // ID2 recebe o id da base;
    evt_add->tempo = (evt->tempo) + duracao; // Tempo recebe agora
    evt_add->tipo = EV_CHEGA;                // Tipo do evento
    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);

    printf("%6d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", evt->tempo, evt->id1,
           w->herois[evt->id1]->base, evt->id2, dist, w->herois[evt->id1]->vel, evt_add->tempo);

    w->herois[evt->id1]->base = -1;
}

void remove_eventos(struct mundo *w, int heroi, int invasao)
{
    struct fprio_t *lef_temp = fprio_cria();
    int tipo, tempo;
    struct evento *prim;

    if (invasao)
    {
        while ((prim = fprio_retira(w->lef, &tipo, &tempo)) != NULL)
        {
            struct evento *evt2 = prim;
            if ((evt2->id1 == heroi) && (evt2->tipo == EV_SAI))
                free(evt2);
            else
                fprio_insere(lef_temp, evt2, evt2->tipo, evt2->tempo);
        }

        while ((prim = fprio_retira(lef_temp, &tipo, &tempo)) != NULL)
            fprio_insere(w->lef, prim, prim->tipo, prim->tempo);
    }
    else
    {
        while ((prim = fprio_retira(w->lef, &tipo, &tempo)) != NULL)
        {
            struct evento *evt2 = prim;

            if ((evt2->id1 == heroi) && (evt2->tipo == EV_ESPERA || evt2->tipo == EV_DESISTE || evt2->tipo == EV_VIAJA ||
                                         evt2->tipo == EV_ENTRA || evt2->tipo == EV_SAI || evt2->tipo == EV_RECUPERA_H))
                free(evt2);
            else
                fprio_insere(lef_temp, evt2, evt2->tipo, evt2->tempo);
        }

        while ((prim = fprio_retira(lef_temp, &tipo, &tempo)) != NULL)
            fprio_insere(w->lef, prim, prim->tipo, prim->tempo);
    }

    fprio_destroi(lef_temp);
}

void evento_morre(struct mundo *w, struct evento *evt)
{
    int id_base = w->herois[evt->id1]->base;

    printf("%6d: MORRE HEROI %2d MISSAO %d\n", evt->tempo, evt->id1, evt->id2); // ID2 é a missão

    cjto_retira(w->bases[id_base]->presentes, evt->id1);

    w->herois[evt->id1]->morto = 1;

    remove_eventos(w, evt->id1, 0);

    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

    if (!evt_add) // Se tiver problemas na alocação, retorna
        return;

    evt_add->id1 = id_base;      // ID1 recebe id da base
    evt_add->id2 = -1;           // ID2 recebe -1;
    evt_add->tempo = evt->tempo; // Tempo recebe agora
    evt_add->tipo = EV_AVISA;    // Tipo do evento
    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
}

void evento_missao(struct mundo *w, struct evento *evt)
{
    struct fprio_t *bases = fprio_cria();
    int dist, tipo;
    int bmp = 0;
    int *id_base;
    struct base *b_escolhida = NULL;
    struct cjto_t *hab_b_escolhida = NULL;

    w->missoes[evt->id1]->tentativas++;

    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", evt->tempo, evt->id1, w->missoes[evt->id1]->tentativas);
    cjto_imprime(w->missoes[evt->id1]->hab);
    printf(" ]\n");

    for (int i = 0; i < N_BASES; i++){
        id_base = malloc(sizeof(int));
        *id_base = i;
        dist = distancia(w->bases[i]->local, w->missoes[evt->id1]->local);
        fprio_insere(bases, id_base, 0, dist);
    }

    while ((bmp == 0) && ((id_base = fprio_retira(bases, &tipo, &dist)) != NULL))
    {
        struct base *b = w->bases[*id_base];
        free(id_base);

        struct cjto_t *hab_b = cjto_cria(N_HABILIDADES);

        for (int j = 0; j < N_HEROIS; j++)
        {
            if (cjto_pertence(b->presentes, j) && (w->herois[j]->estado))
            {
                struct cjto_t *aux = cjto_uniao(hab_b, w->herois[j]->hab);
                cjto_destroi(hab_b);
                hab_b = aux;
            }
        }

        if (cjto_contem(hab_b, w->missoes[evt->id1]->hab))
        {
            bmp = 1;
            b_escolhida = b;
            hab_b_escolhida = hab_b;
        }
        else{
            cjto_destroi(hab_b);
        }
    }

    if (bmp)
    {
        w->missoes[evt->id1]->completa = 1;
        for (int j = 0; j < N_HEROIS; j++)
        {
            if (cjto_pertence(b_escolhida->presentes, j) && (w->herois[j]->estado)) // aqui? tempo 2188
            {
                int risco = w->missoes[evt->id1]->perigo / (w->herois[j]->paciencia + w->herois[j]->exp + 1);

                if (risco > aleat(1, 30))
                {
                    struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado
                    evt_add->id1 = j;            // ID1 recebe id do heroi
                    evt_add->id2 = evt->id1;     // ID2 recebe -1;
                    evt_add->tempo = evt->tempo; // Tempo recebe agora
                    evt_add->tipo = EV_MORRE;    // Tipo do evento
                    fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
                }
                else
                    w->herois[j]->exp++;
            }
        }

        printf("%6d: MISSAO %d CUMPRIDA BASE %d HAB [ ", evt->tempo, evt->id1, b_escolhida->id);
        cjto_imprime(hab_b_escolhida);
        printf(" ]\n");

        b_escolhida->missoes++;

        cjto_destroi(hab_b_escolhida);
    }
    else
    {
        struct evento *evt_m = malloc(sizeof(struct evento)); // Aloca memória para o evento
        evt_m->id1 = evt->id1;                                // ID1 recebe o id da missão
        evt_m->id2 = -1;                                      // ID2 é -1
        evt_m->tempo = evt->tempo + (24 * 60);                // Instante que vai acontecer
        evt_m->tipo = EV_MISSAO;                              // Tipo do evento

        fprio_insere(w->lef, evt_m, evt_m->tipo, evt_m->tempo);

        printf("%6d: MISSAO %d IMPOSSIVEL\n", evt->tempo, evt->id1);
    }

    fprio_destroi(bases);
}

void evento_invade(struct mundo *w, struct evento *evt)
{
    int id_base = w->invasoes[evt->id1]->local;

    if (w->bases[id_base]->estado)
    {
        w->invasoes_realizadas++;
        printf("%6d: INVADE BASE %d INVADE %d\n", evt->tempo, id_base, evt->id1);

        for (int i = 0; i < N_HEROIS; i++)
        {
            if (cjto_pertence(w->bases[id_base]->presentes, i))
            {
                w->herois[i]->estado = 0;

                remove_eventos(w, i, 1);

                struct evento *evt_add1 = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

                if (!evt_add1) // Se tiver problemas na alocação, retorna
                    return;

                evt_add1->id1 = i;                                           // ID1 recebe id do herói
                evt_add1->id2 = -1;                                          // ID2 recebe -1;
                evt_add1->tempo = (evt->tempo) + w->invasoes[evt->id1]->tph; // Tempo recebe agora + tph
                evt_add1->tipo = EV_RECUPERA_H;                              // Tipo do evento
                fprio_insere(w->lef, evt_add1, evt_add1->tipo, evt_add1->tempo);

                struct evento *evt_add2 = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

                if (!evt_add2) // Se tiver problemas na alocação, retorna
                    return;

                evt_add2->id1 = i;            // ID1 recebe id do herói
                evt_add2->id2 = id_base;      // ID2 recebe o id da base;
                evt_add2->tempo = evt->tempo; // Tempo recebe agora
                evt_add2->tipo = EV_SAI;      // Tipo do evento
                fprio_insere(w->lef, evt_add2, evt_add2->tipo, evt_add2->tempo);
            }
        }
        printf("%6d: INVADE BASE %d HEROIS INATIVOS [ ", evt->tempo, w->invasoes[evt->id1]->local);
        cjto_imprime(w->bases[w->invasoes[evt->id1]->local]->presentes);
        printf(" ]\n");

        w->bases[id_base]->estado = 0;
        printf("%6d: INVADE BASE %d INATIVA\n", evt->tempo, w->invasoes[evt->id1]->local);

        struct evento *evt_add = malloc(sizeof(struct evento)); // Aloca memória para o evento a se adicionado

        if (!evt_add) // Se tiver problemas na alocação, retorna
            return;

        evt_add->id1 = id_base;                                     // ID1 recebe id da base
        evt_add->id2 = -1;                                          // ID2 recebe -1;
        evt_add->tempo = (evt->tempo) + w->invasoes[evt->id1]->tpb; // Tempo recebe agora + tph
        evt_add->tipo = EV_RECUPERA_B;                              // Tipo do evento
        fprio_insere(w->lef, evt_add, evt_add->tipo, evt_add->tempo);
    }
}

void evento_recupera_h(struct mundo *w, struct evento *evt)
{
    w->herois[evt->id1]->estado = 1;

    printf("%6d: RECUPERA HEROI %2d\n", evt->tempo, evt->id1);
}

void evento_recupera_b(struct mundo *w, struct evento *evt)
{
    w->bases[evt->id1]->estado = 1;

    printf("%6d: RECUPERA BASE %d\n", evt->tempo, evt->id1);
}

void evento_fim(struct mundo *w, struct evento *evt)
{
    int i, min_missoes, max_missoes;
    int mortos = 0, missoes_cumpr = 0, soma = 0;
    double media, taxa_missoes_cumpr, taxa_invasoes, taxa_mort;
    w->relogio = T_FIM_DO_MUNDO + 1; // Seta o relógio para além do final

    printf("%6d: FIM\n", evt->tempo);

    for (i = 0; i < N_HEROIS; i++)
    { // Percorre os heróis contando os mortos
        if (w->herois[i]->morto)
        {
            mortos++;
            printf("HEROI %2d MORTO PAC %3d VEL %4d EXP %4d HABS [ ", i, w->herois[i]->paciencia,
                   w->herois[i]->vel, w->herois[i]->exp);
            cjto_imprime(w->herois[i]->hab);
            printf(" ]\n");
        }
        else
        {
            printf("HEROI %2d VIVO PAC %3d VEL %4d EXP %4d HABS [ ", i, w->herois[i]->paciencia,
                   w->herois[i]->vel, w->herois[i]->exp);
            cjto_imprime(w->herois[i]->hab);
            printf(" ]\n");
        }
    }

    for (i = 0; i < N_BASES; i++)
    { // Percorre as bases contando as missoes cumpridas
        missoes_cumpr += w->bases[i]->missoes;
        printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", i, w->bases[i]->lotacao,
               w->bases[i]->max, w->bases[i]->missoes);
    }

    printf("EVENTOS TRATADOS: %d\n", w->eventos_tratados);

    taxa_missoes_cumpr = 100.0 * missoes_cumpr / N_MISSOES;
    printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", missoes_cumpr, N_MISSOES, taxa_missoes_cumpr);

    taxa_invasoes = 100.0 * w->invasoes_realizadas / N_INVASOES;
    printf("INVASÕES REALIZADAS: %d/%d (%.1f%%)\n", w->invasoes_realizadas, N_INVASOES, taxa_invasoes);

    // Inicializações para os cálculos das missões
    min_missoes = w->missoes[0]->tentativas;
    max_missoes = w->missoes[0]->tentativas;
    soma += w->missoes[0]->tentativas;

    for (i = 1; i < N_MISSOES; i++)
    { // Calcula a menor, a maior e a soma das tentativas feitas nas missões
        soma += w->missoes[i]->tentativas;
        if (w->missoes[i]->tentativas < min_missoes)
            min_missoes = w->missoes[i]->tentativas;
        if (w->missoes[i]->tentativas > max_missoes)
            max_missoes = w->missoes[i]->tentativas;
    }
    media = (double)soma / N_MISSOES; // Calcula a média de tentativas

    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", min_missoes, max_missoes, media);

    taxa_mort = 100.0 * mortos / N_HEROIS;
    printf("TAXA MORTALIDADE: %.1f%%\n", taxa_mort);
}