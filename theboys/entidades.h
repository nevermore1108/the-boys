#ifndef ENTIDADES
#define ENTIDADES
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO (30 * 24 * 60) //(30*24*60) 525600 = 60*24*365 = 365 dias
#define N_TAMANHO_MUNDO 1000          // 20000 1000
#define N_HABILIDADES 10
#define N_HEROIS (N_HABILIDADES * 5)
#define N_BASES (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)
#define N_INVASOES (T_FIM_DO_MUNDO / 10000)

struct posicao
{
    int x;
    int y;
};

struct heroi
{
    int id;             // ID do herói
    struct cjto_t *hab; // Conjunto de habilidades do herói
    int estado;         // Estado atual do herói, 1 (ativo) e 0 (inativo)
    int morto;          // 0 se o herói estiver vivo e 1 se morto
    int paciencia;      // Nível de paciência do herói
    int vel;            // Velocidade do herói
    int exp;            // Quantidade de missões que o herói participou, experiência
    int base;           // ID da base que o herói está, -1 se em nenhuma base
};

struct base
{
    int id;                   // ID da base
    int lotacao;              // Capacidade máxima da base
    struct cjto_t *presentes; // Conjunto de heróis presentes na base
    int estado;               // Estado atual da base, 1 (ativa) e 0 (inativa)
    struct fila_t *espera;    // Fila de espera da base
    struct posicao local;     // Localização, coordenadas X e Y, da base no mundo
    int missoes;              // Quantidade de missóes participadas pela base
    int max;                  // Maior fila de espera atingida na base
};

struct missao
{
    int id;               // ID da missão
    struct cjto_t *hab;   // Conjunto de habilidades necessárias para completar a missão
    int perigo;           // Nível de perigo da missão
    struct posicao local; // Local, coordenadas X e Y, que a missão vai acontecer
    int completa;         // Missão foi cumprida (1) ou não (0)
    int tentativas;       // Quantas tentativas a missão teve
};

struct invasao
{
    int id;    // ID da invasão
    int local; // ID da base que vai ocorrer a invasão
    int tpb;   // Tempo de penalidade da base
    int tph;   // Tempo de penalidade do herói
};

struct mundo
{
    struct heroi *herois[N_HEROIS];       // Vetor com todos os heróis
    struct base *bases[N_BASES];          // Vetor com todas as bases
    struct missao *missoes[N_MISSOES];    // Vetor com todas as missões
    struct invasao *invasoes[N_INVASOES]; // Vetor com todas as invasões
    struct posicao tamanho;               // Tamanho máximo do mundo
    int relogio;                          // Tempo atual no mundo
    int eventos_tratados;                 // Quantidade de eventos tratados na simulação
    int invasoes_realizadas;              // Invasões que foram realizadas
    struct fprio_t *lef;                  // Lista de Eventos Futuros
};

/*Cria o mundo, com seus heróis, bases, missões e invasões definidos
|*Retorno: ponteiro para o mundo criado ou NULL se erro
|*Nota: Eu gostaria muito de ter chamado essa função de "big_bang" :( */
struct mundo *cria_mundo();

/*Destrói o mundo criado, desalocando e liberando a memória utilizada
|*Retorno: NULL
|*Nota: Eu gostaria muito de ter chamado essa função de
|*"heat_death_of_the_universe" :( */
struct mundo *destroi_mundo(struct mundo *w);

#endif