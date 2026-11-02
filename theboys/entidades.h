#ifndef ENTIDADES
#define ENTIDADES
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600 // 525600 = 60*24*365 = 365 dias
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS (N_HABILIDADES * 5)
#define N_BASES (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)
#define N_INVASOES (T_FIM_DO_MUNDO / 10000)

struct posicao {
    int x;
    int y;
};

struct heroi {
    int id; //ID do herói
    struct cjto_t *hab; //Conjunto de habilidades
    int estado; //Estado atual do herói, 1 (ativo) e 0 (inativo)
    int paciencia; //Nível de paciência do herói
    int vel; //Velocidade do herói
    int exp; //Experiência do herói
    int base; //ID da base que o herói está
    int vivo; //Herói está vivo (1) ou morto (0)
};

struct base {
    int id; //ID da base
    int lotacao; //Capacidade máxima da base
    struct cjto_t *presentes; //Conjunto de heróis presentes na base
    int estado; //Estado atual da base, 1 (ativa) e 0 (inativa)
    struct fila_t *espera; //Fila de espera da base
    struct posicao local; //Localização da base no mundo
    int missoes; //Quantidade de missóes participadas
    int max; //Maior fila atingida na base
};

struct missao {
    int id; //ID da missão
    struct cjto_t *hab; //Conjunto de habilidades para completar a missão
    int perigo; //Nível de perigo da missão
    struct posicao local; //Local que a missão vai acontecer
    int completa; //Missão cumprida (1) ou não (0)
    int tentativas; //Quantas tentativas teve
};

struct invasao {
    int id; //ID da invasão
    int local; //ID da base que vai ocorrer
    int tpb; //Tempo de penalidade da base
    int tph; //Tempo de penalidade do herói
    int realizada; //Invasão realizada (1) ou não (0)
};

struct mundo {
    struct heroi *heroi[N_HEROIS]; //Vetor com todos os heróis
    struct base *base[N_BASES]; //Vetor com todas as bases
    struct missao *missao[N_MISSOES]; //Vetor com todas as missões
    struct invasao *invasao[N_INVASOES]; //Vetor com todas as invasões
    struct posicao tamanho; //Tamanho máximo do mundo
    int relogio; //Tempo atual no mundo
    int eventos_tratados; //Quantidade de eventos tratados
    struct fprio_t *lef; //Lista de Eventos Futuros 
};

#endif