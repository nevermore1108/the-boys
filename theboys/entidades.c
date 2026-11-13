#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"
#include "entidades.h"
#include "auxiliares.h"
#include "eventos.h"

//Cria um herói com seus atributos
//Retorno: ponteiro para o herói ou NULL se erro
struct heroi *cria_heroi (int id){
    struct heroi *h = malloc (sizeof (struct heroi)); //Aloca memória para o herói

    if (!h) //Se tiver problemas na alocação, retorna NULL
        return NULL;
    
    h->id = id; //Recebe o id
    h->estado = 1; //Começa ativo
    h->morto = 0; //Começa vivo
    h->paciencia = aleat (1, 100); //Paciência aleatória
    h->vel = aleat (50, 5000); //Velocidade Aleatória
    h->exp = 0; //Começa com 0 de exp
    h->base = -1; //Começa fora de qualquer base
    
    h->hab = cjto_aleat(aleat(1, 3), N_HABILIDADES); //Cria o conjunto das habilidades
    if (!h->hab){ //Se tiver problemas na alocação, libera e retorna NULL
        free(h);
        return NULL;
    }

    return h;
}

//Cria uma base com seus atributos
//Retorno: ponteiro para a base ou NULL se erro
struct base *cria_base (int id){
    struct base *b = malloc (sizeof (struct base)); //Aloca memória para a base

    if (!b) //Se tiver problemas na alocação, retorna NULL
        return NULL;
    
    b->id = id; //Recebe o id

    //Recebe suas coordenadas X e Y
    b->local.x = aleat (0, N_TAMANHO_MUNDO-1);
    b->local.y = aleat (0, N_TAMANHO_MUNDO-1);

    b->lotacao = aleat (3, 10); //Lotação máxima aleatória
    b->estado = 1; //Começa ativo
    b->missoes = 0; //Começa com 0 missões participadas
    b->max = 0; //Maior fila de espera encontrada na base, começa em 0

    b->presentes = cjto_cria(N_HEROIS); //Cria o conjunto de heróis presentes
    if (!b->presentes){ //Se tiver problemas na alocação, libera e retorna NULL
        cjto_destroi(b->presentes);
        free(b);
        return NULL;
    }

    b->espera = fila_cria (); //Cria a fila de espera vazia
    if (!b->espera){ //Se tiver problemas na alocação, libera e retorna NULL
        free(b);
        return NULL;
    }

    return b;
}

//Cria uma missão com seus atributos
//Retorno: ponteiro para a missão ou NULL se erro
struct missao *cria_missao (int id){
    struct missao *m = malloc (sizeof (struct missao)); //Aloca memória para a missao

    if (!m) //Se tiver problemas na alocação, retorna NULL
        return NULL;
    
    m->id = id; //Recebe o id

    //Recebe suas coordenadas X e Y
    m->local.x = aleat (0, N_TAMANHO_MUNDO-1);
    m->local.y = aleat (0, N_TAMANHO_MUNDO-1);

    m->perigo = aleat (1, 100); //Perigo aleatório
    m->completa = 0; //Começa incompleta
    m->tentativas = 0; //0 tentativas na missão

    m->hab = cjto_aleat(aleat(6, 10), N_HABILIDADES); //Cria o conjunto das habilidades
    if (!m->hab){ //Se tiver problemas na alocação, libera e retorna NULL
        free(m);
        return NULL;
    }

    return m;
}

//Cria uma invasão com seus atributos
//Retorno: ponteiro para a invasão ou NULL se erro
struct invasao *cria_invasao (int id){
    struct invasao *i = malloc (sizeof (struct invasao)); //Aloca memória para a missao

    if (!i) //Se tiver problemas na alocação, retorna NULL
        return NULL;
    
    i->id = id; //Recebe o id
    i->local = aleat (0, N_BASES-1); //Seleciona a base que vai ocorrer aleatóriamente

    //Tempos de penalidade aleatórios
    i->tph = aleat (0, 4320);
    i->tpb = aleat (1, 7200);

    return i;
}

//Inicializa os primeiros eventos do mundo
void eventos_iniciais (struct mundo *w){
    int i;

    for (i = 0; i < N_HEROIS; i++){ //Eventos iniciais dos heróis (chega)
        struct evento *evt_h = malloc (sizeof (struct evento)); //Aloca memória para o evento
        evt_h->id1 = i; //ID1 recebe id do herói
        evt_h->id2 = aleat (0, N_BASES-1); //ID2 recebe id da base, decidida aleatóriamente
        evt_h->tempo = aleat (0, 4320); //Instante que vai chegar
        evt_h->tipo = EV_CHEGA; //Tipo do evento

        fprio_insere(w->lef, evt_h,evt_h->tipo,evt_h->tempo); //Insere na lef
    }

    for (i = 0; i < N_MISSOES; i++){ //Todas as missões são colocadas na lef
        struct evento *evt_m = malloc (sizeof (struct evento)); //Aloca memória para o evento
        evt_m->id1 = i; //ID1 recebe o id da missão
        evt_m->id2 = -1; //ID2 é -1 (inválido pq não está sendo usado)
        evt_m->tempo = aleat (0, T_FIM_DO_MUNDO); //Instante que vai acontecer, decidido aleatóriamente
        evt_m->tipo = EV_MISSAO; //Tipo do evento

        fprio_insere(w->lef, evt_m,evt_m->tipo,evt_m->tempo); //Insere na lef
    }

    for (i = 0; i < N_INVASOES; i++){ //Todas as invasões são colocadas na lef
        struct evento *evt_i = malloc (sizeof (struct evento)); //Aloca memória para o evento
        evt_i->id1 = i; //ID1 recebe o id da invasão
        evt_i->id2 = -1; //ID2 é -1 (inválido pq não está sendo usado)
        evt_i->tempo = aleat ((60*24*3)+1, T_FIM_DO_MUNDO); //Instante que vai acontecer, decidido aleatóriamente
        evt_i->tipo = EV_INVADE; //Tipo do evento

        fprio_insere(w->lef, evt_i,evt_i->tipo,evt_i->tempo); //Insere na lef
    }

    //Insere o fim no final da lef
    struct evento *evt_f = malloc (sizeof (struct evento)); //Aloca memória para o evento
    evt_f->id1 = -1; //ID1 é -1 (inválido pq não está sendo usado)
    evt_f->id2 = -1; //ID2 é -1 (inválido pq não está sendo usado)
    evt_f->tempo = T_FIM_DO_MUNDO; //Instante final, no qual esse evento vai acontecer
    evt_f->tipo = EV_FIM; //Tipo do evento

    fprio_insere(w->lef, evt_f,evt_f->tipo,evt_f->tempo); //Insere na lef

    return;
}

//Cria o mundo, com seus heróis, bases, missões e invasões definidos
//Retorno: ponteiro para o mundo criado ou NULL se erro.
struct mundo *cria_mundo (){
    int i;
    struct mundo *w = malloc (sizeof (struct mundo)); //Aloca memória para o mundo

    if (!w) //Se tiver problemas na alocação, retorna NULL
        return NULL;

    //Inicializa o tamanho do mundo (coordenadas X e Y)
    w->tamanho.x = N_TAMANHO_MUNDO;
    w->tamanho.y = N_TAMANHO_MUNDO;

    w->relogio = T_INICIO; //Começa com o relógio em 0 (não vai mudar até chegar no fim)
    w->eventos_tratados = 0; //Quantidade de eventos tratados começa em 0
    w->invasoes_realizadas = 0; //Quantidade de eventos realizados começa em 0
    w->lef = fprio_cria(); //Cria a lef

    for (i = 0; i < N_HEROIS; i++) //Cria os heróis
        w->herois[i] = cria_heroi(i);

    for (i = 0; i < N_BASES; i++) //Cria as bases
        w->bases[i] = cria_base(i);
    
    for (i = 0; i < N_MISSOES; i++) //Cria as missões
        w->missoes[i] = cria_missao(i);
    
    for (i = 0; i < N_INVASOES; i++) //Cria as invasões
        w->invasoes[i] = cria_invasao(i);

    eventos_iniciais(w); //Insere os eventos iniciais

    return w;
}

//Destrói o mundo criado, desalocando e liberando a memória utilizada 
//Retorno: NULL
struct mundo *destroi_mundo (struct mundo *w){
    int i;
    if (!w) //Se não existir mundo, retorna
        return NULL;

    for (i = 0; i < N_HEROIS; i++){ //Libera os heróis
        if (w->herois[i]){
            if (w->herois[i]->hab) //Destroi o conjunto de habilidades
                w->herois[i]->hab = cjto_destroi(w->herois[i]->hab);

            free(w->herois[i]);
        }
    }

    for (i = 0; i < N_BASES; i++){ //Libera as bases
        if (w->bases[i]){
            if (w->bases[i]->presentes) //Destroi o conjunto de presentes
                w->bases[i]->presentes = cjto_destroi(w->bases[i]->presentes);

            if (w->bases[i]->espera) //Destroi a fila de espera
                w->bases[i]->espera = fila_destroi(w->bases[i]->espera);

            free(w->bases[i]);
        }
    }

    for (i = 0; i < N_MISSOES; i++){ //Libera as missões
        if (w->missoes[i]){
            if (w->missoes[i]->hab) //Destroi o conjunto de habilidades
                w->missoes[i]->hab = cjto_destroi(w->missoes[i]->hab);

            free(w->missoes[i]);
        }
    }

    for (i = 0; i < N_INVASOES; i++){ //Libera as invasões
        if (w->invasoes[i])
            free(w->invasoes[i]);
    }

    w->lef = fprio_destroi(w->lef); //Destroi a lef
    free(w);

    return NULL;
}