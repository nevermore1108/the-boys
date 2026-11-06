// Programa de teste do entidades.c (fora da pasta de entrega!)
//
// Compilar (testes funcionais):
//   gcc -Wall -Werror -Wextra -std=c99 -g teste_entidades.c entidades.c auxiliares.c fprio.c fila.c conjunto.o -o teste_entidades
//
// Compilar (com injecao de falhas de alocacao, opcional):
//   gcc -Wall -Werror -Wextra -std=c99 -g -DINJETA_FALHAS teste_entidades.c entidades.c auxiliares.c fprio.c fila.c conjunto.o -Wl,--wrap=malloc -Wl,--wrap=calloc -o teste_entidades
//
// Rodar:
//   ./teste_entidades
//   valgrind --leak-check=full --show-leak-kinds=all ./teste_entidades

#include <stdio.h>
#include <stdlib.h>
#include "conjunto.h"
#include "fila.h"
#include "fprio.h"
#include "entidades.h"
#include "auxiliares.h"

// construtores individuais (nao estao declarados em entidades.h)
struct heroi *cria_heroi (int id) ;
struct base *cria_base (int id) ;
struct missao *cria_missao (int id) ;
struct invasao *cria_invasao (int id) ;

// ------------------------------------------------------------ infraestrutura

#define MAX_LINHAS 2048

static int total = 0 ;
static int falhas = 0 ;
static int limite = 3 ;			// maximo de mensagens por linha de teste
static int ocultas[MAX_LINHAS] ;
static int cont_linha[MAX_LINHAS] ;

static void checa (int cond, const char *msg, int idx, int linha)
{
	total++ ;
	if (cond)
		return ;
	falhas++ ;
	if (linha >= 0 && linha < MAX_LINHAS)
	{
		cont_linha[linha]++ ;
		if (cont_linha[linha] > limite)
		{
			ocultas[linha]++ ;
			return ;
		}
	}
	if (idx >= 0)
		printf ("  [FALHOU] %s (indice %d, linha %d)\n", msg, idx, linha) ;
	else
		printf ("  [FALHOU] %s (linha %d)\n", msg, linha) ;
}

#define CHECA(cond, msg)	checa ((cond), (msg), -1, __LINE__)
#define CHECAI(cond, msg, i)	checa ((cond), (msg), (i), __LINE__)

// conta os flags verdadeiros de um conjunto
static int conta (struct cjto_t *c)
{
	int n = 0 ;
	for (int i = 0 ; i < c->cap ; i++)
		if (c->flag[i])
			n++ ;
	return n ;
}

// verifica um conjunto: capacidade, cardinalidade e consistencia interna
static void checa_cjto (struct cjto_t *c, int cap, int cmin, int cmax,
			const char *quem, int idx, int linha)
{
	char msg[160] ;
	snprintf (msg, sizeof (msg), "%s: conjunto nao criado", quem) ;
	checa (c != NULL, msg, idx, linha) ;
	if (!c)
		return ;
	snprintf (msg, sizeof (msg), "%s: capacidade do conjunto deveria ser %d (e %d)",
		  quem, cap, c->cap) ;
	checa (c->cap == cap, msg, idx, linha) ;
	snprintf (msg, sizeof (msg), "%s: cardinalidade %d fora da faixa [%d...%d]",
		  quem, c->num, cmin, cmax) ;
	checa (c->num >= cmin && c->num <= cmax, msg, idx, linha) ;
	snprintf (msg, sizeof (msg), "%s: cardinalidade (%d) diferente do numero de flags (%d)",
		  quem, c->num, conta (c)) ;
	checa (c->num == conta (c), msg, idx, linha) ;
}

#ifdef INJETA_FALHAS
// ---- injecao de falhas: faz o N-esimo malloc/calloc (a partir de armar) falhar
extern void *__real_malloc (size_t) ;
extern void *__real_calloc (size_t, size_t) ;

static int armado = 0 ;
static int falha_em = 0 ;
static int contagem = 0 ;
static int disparou = 0 ;

static void armar (int n)
{
	falha_em = n ;
	contagem = 0 ;
	disparou = 0 ;
	armado = 1 ;
}

static void desarmar ()
{
	armado = 0 ;
}

static int deve_falhar ()
{
	if (!armado)
		return 0 ;
	contagem++ ;
	if (contagem == falha_em)
	{
		disparou = 1 ;
		return 1 ;
	}
	return 0 ;
}

void *__wrap_malloc (size_t n)
{
	if (deve_falhar ())
		return NULL ;
	return __real_malloc (n) ;
}

void *__wrap_calloc (size_t a, size_t b)
{
	if (deve_falhar ())
		return NULL ;
	return __real_calloc (a, b) ;
}
#endif

// ------------------------------------------------------------ teste: aleat

static void testa_aleat_faixa (long min, long max)
{
	int n = 200000 ;
	int fora = 0, viu_min = 0, viu_max = 0 ;
	char msg[160] ;

	for (int i = 0 ; i < n ; i++)
	{
		long v = aleat (min, max) ;
		if (v < min || v > max)
			fora++ ;
		if (v == min)
			viu_min = 1 ;
		if (v == max)
			viu_max = 1 ;
	}
	snprintf (msg, sizeof (msg), "aleat(%ld,%ld): %d de %d valores fora da faixa",
		  min, max, fora, n) ;
	CHECA (fora == 0, msg) ;
	snprintf (msg, sizeof (msg), "aleat(%ld,%ld): nunca devolveu o minimo", min, max) ;
	CHECA (viu_min, msg) ;
	snprintf (msg, sizeof (msg), "aleat(%ld,%ld): nunca devolveu o maximo", min, max) ;
	CHECA (viu_max, msg) ;
}

static void testa_aleat ()
{
	printf ("aleat: faixas usadas no projeto\n") ;
	limite = 100 ;
	srand (12345) ;
	testa_aleat_faixa (0, 0) ;
	testa_aleat_faixa (1, 3) ;
	testa_aleat_faixa (6, 10) ;
	testa_aleat_faixa (0, 100) ;
	testa_aleat_faixa (3, 10) ;
	testa_aleat_faixa (50, 5000) ;
	testa_aleat_faixa (0, 4320) ;
	testa_aleat_faixa (0, N_TAMANHO_MUNDO - 1) ;
	limite = 3 ;
}

// ------------------------------------------------------------ teste: macros

static void testa_macros ()
{
	printf ("macros: valores configurados\n") ;
	printf ("  N_HEROIS=%d N_BASES=%d N_MISSOES=%d N_INVASOES=%d\n",
		N_HEROIS, N_BASES, N_MISSOES, N_INVASOES) ;
	CHECA (N_HEROIS == N_HABILIDADES * 5, "N_HEROIS deve ser N_HABILIDADES * 5") ;
	CHECA (N_BASES == N_HEROIS / 5, "N_BASES deve ser N_HEROIS / 5") ;
	CHECA (N_MISSOES == T_FIM_DO_MUNDO / 100, "N_MISSOES deve ser T_FIM_DO_MUNDO / 100") ;
	CHECA (N_INVASOES == T_FIM_DO_MUNDO / 10000, "N_INVASOES deve ser T_FIM_DO_MUNDO / 10000") ;
	if (T_FIM_DO_MUNDO != 525600)
		printf ("  AVISO: T_FIM_DO_MUNDO=%d (enunciado: 525600). Volte ao valor final antes de entregar.\n",
			T_FIM_DO_MUNDO) ;
	if (N_TAMANHO_MUNDO != 20000)
		printf ("  AVISO: N_TAMANHO_MUNDO=%d (enunciado: 20000).\n", N_TAMANHO_MUNDO) ;
}

// ------------------------------------------------------------ teste: mundo

static int distintos (int *v, int n)
{
	for (int i = 1 ; i < n ; i++)
		if (v[i] != v[0])
			return 1 ;
	return 0 ;
}

static void verifica_herois (struct mundo *w)
{
	int pac[N_HEROIS], vel[N_HEROIS] ;

	for (int i = 0 ; i < N_HEROIS ; i++)
	{
		struct heroi *h = w->herois[i] ;
		CHECAI (h != NULL, "heroi nao criado", i) ;
		if (!h)
		{
			pac[i] = vel[i] = 0 ;
			continue ;
		}
		CHECAI (h->id == i, "id do heroi incorreto", i) ;
		CHECAI (h->estado == 1, "heroi deve comecar ativo (1)", i) ;
		CHECAI (h->paciencia >= 0 && h->paciencia <= 100, "paciencia fora de [0...100]", i) ;
		CHECAI (h->vel >= 50 && h->vel <= 5000, "velocidade fora de [50...5000]", i) ;
		CHECAI (h->exp == 0, "experiencia deve comecar em 0", i) ;
		CHECAI (h->base == -1, "heroi deve comecar sem base (-1)", i) ;
		checa_cjto (h->hab, N_HABILIDADES, 1, 3, "habilidades do heroi", i, __LINE__) ;
		pac[i] = h->paciencia ;
		vel[i] = h->vel ;
		for (int j = 0 ; j < i ; j++)
			if (w->herois[j] && h->hab && h->hab == w->herois[j]->hab)
				CHECAI (0, "dois herois compartilham o mesmo conjunto de habilidades", i) ;
	}
	CHECA (distintos (pac, N_HEROIS), "paciencia dos herois parece constante (aleatoriedade?)") ;
	CHECA (distintos (vel, N_HEROIS), "velocidade dos herois parece constante (aleatoriedade?)") ;
}

static void verifica_bases (struct mundo *w)
{
	int px[N_BASES], py[N_BASES], lot[N_BASES] ;

	for (int i = 0 ; i < N_BASES ; i++)
	{
		struct base *b = w->bases[i] ;
		CHECAI (b != NULL, "base nao criada", i) ;
		if (!b)
		{
			px[i] = py[i] = lot[i] = 0 ;
			continue ;
		}
		CHECAI (b->id == i, "id da base incorreto", i) ;
		CHECAI (b->lotacao >= 3 && b->lotacao <= 10, "lotacao fora de [3...10]", i) ;
		CHECAI (b->estado == 1, "base deve comecar ativa (1)", i) ;
		CHECAI (b->local.x >= 0 && b->local.x <= N_TAMANHO_MUNDO - 1, "coordenada X da base fora do mundo", i) ;
		CHECAI (b->local.y >= 0 && b->local.y <= N_TAMANHO_MUNDO - 1, "coordenada Y da base fora do mundo", i) ;
		CHECAI (b->missoes == 0, "contador de missoes da base deve comecar em 0", i) ;
		CHECAI (b->max == 0, "fila maxima da base deve comecar em 0", i) ;
		checa_cjto (b->presentes, b->lotacao, 0, 0, "presentes da base", i, __LINE__) ;
		CHECAI (b->espera != NULL, "fila de espera da base nao criada", i) ;
		if (b->espera)
			CHECAI (fila_tamanho (b->espera) == 0, "fila de espera deve comecar vazia", i) ;
		px[i] = b->local.x ;
		py[i] = b->local.y ;
		lot[i] = b->lotacao ;
		for (int j = 0 ; j < i ; j++)
			if (w->bases[j] && b->presentes && b->presentes == w->bases[j]->presentes)
				CHECAI (0, "duas bases compartilham o mesmo conjunto de presentes", i) ;
	}
	CHECA (distintos (px, N_BASES), "coordenada X das bases parece constante (aleatoriedade?)") ;
	CHECA (distintos (py, N_BASES), "coordenada Y das bases parece constante (aleatoriedade?)") ;
	CHECA (distintos (lot, N_BASES), "lotacao das bases parece constante (aleatoriedade?)") ;
}

static void verifica_missoes (struct mundo *w)
{
	int *per = malloc (N_MISSOES * sizeof (int)) ;
	int *mx = malloc (N_MISSOES * sizeof (int)) ;
	if (!per || !mx)
	{
		printf ("  sem memoria no teste\n") ;
		exit (1) ;
	}

	for (int i = 0 ; i < N_MISSOES ; i++)
	{
		struct missao *m = w->missoes[i] ;
		CHECAI (m != NULL, "missao nao criada", i) ;
		if (!m)
		{
			per[i] = mx[i] = 0 ;
			continue ;
		}
		CHECAI (m->id == i, "id da missao incorreto", i) ;
		CHECAI (m->perigo >= 0 && m->perigo <= 100, "perigo fora de [0...100]", i) ;
		CHECAI (m->local.x >= 0 && m->local.x <= N_TAMANHO_MUNDO - 1, "coordenada X da missao fora do mundo", i) ;
		CHECAI (m->local.y >= 0 && m->local.y <= N_TAMANHO_MUNDO - 1, "coordenada Y da missao fora do mundo", i) ;
		CHECAI (m->completa == 0, "missao deve comecar nao cumprida (0)", i) ;
		CHECAI (m->tentativas == 0, "tentativas da missao devem comecar em 0", i) ;
		checa_cjto (m->hab, N_HABILIDADES, 6, 10, "habilidades da missao", i, __LINE__) ;
		per[i] = m->perigo ;
		mx[i] = m->local.x ;
	}
	CHECA (distintos (per, N_MISSOES), "perigo das missoes parece constante (aleatoriedade?)") ;
	CHECA (distintos (mx, N_MISSOES), "coordenada X das missoes parece constante (aleatoriedade?)") ;
	free (per) ;
	free (mx) ;
}

static void verifica_invasoes (struct mundo *w)
{
	for (int i = 0 ; i < N_INVASOES ; i++)
	{
		struct invasao *v = w->invasoes[i] ;
		CHECAI (v != NULL, "invasao nao criada", i) ;
		if (!v)
			continue ;
		CHECAI (v->id == i, "id da invasao incorreto", i) ;
		CHECAI (v->local >= 0 && v->local <= N_BASES - 1, "base da invasao fora de [0...N_BASES-1]", i) ;
		CHECAI (v->tph >= 0 && v->tph <= 4320, "tempo de penalidade do heroi fora de [0...4320]", i) ;
		CHECAI (v->tpb >= 0 && v->tpb <= 7200, "tempo de penalidade da base fora de [0...7200]", i) ;
		CHECAI (v->realizada == 0, "invasao deve comecar nao realizada (0)", i) ;
	}
}

static void testa_mundo ()
{
	printf ("mundo: criacao e valores iniciais\n") ;
	struct mundo *w = cria_mundo () ;
	CHECA (w != NULL, "cria_mundo devolveu NULL") ;
	if (!w)
		return ;

	CHECA (w->tamanho.x == N_TAMANHO_MUNDO && w->tamanho.y == N_TAMANHO_MUNDO, "tamanho do mundo incorreto") ;
	CHECA (w->relogio == T_INICIO, "relogio deve comecar em T_INICIO") ;
	CHECA (w->eventos_tratados == 0, "eventos_tratados deve comecar em 0") ;
	CHECA (w->mortos == 0, "mortos deve comecar em 0 (malloc nao zera a memoria)") ;
	CHECA (w->lef != NULL, "LEF nao criada") ;
	if (w->lef)
		CHECA (fprio_tamanho (w->lef) >= 0, "LEF invalida") ;

	verifica_herois (w) ;
	verifica_bases (w) ;
	verifica_missoes (w) ;
	verifica_invasoes (w) ;

	CHECA (destroi_mundo (w) == NULL, "destroi_mundo deve devolver NULL") ;
}

static void testa_destroi_nulo ()
{
	printf ("mundo: destroi_mundo(NULL)\n") ;
	CHECA (destroi_mundo (NULL) == NULL, "destroi_mundo(NULL) deve devolver NULL") ;
}

static void testa_repeticao ()
{
	printf ("mundo: criar e destruir varias vezes (veja no valgrind)\n") ;
	struct mundo *a = cria_mundo () ;
	struct mundo *b = cria_mundo () ;
	CHECA (a != NULL && b != NULL, "cria_mundo falhou ao criar dois mundos") ;
	if (a && b)
	{
		CHECA (a != b, "dois mundos devem ser objetos distintos") ;
		CHECA (a->herois[0] != b->herois[0], "mundos nao podem compartilhar herois") ;
		CHECA (a->lef != b->lef, "mundos nao podem compartilhar a LEF") ;
	}
	destroi_mundo (a) ;
	destroi_mundo (b) ;
	for (int i = 0 ; i < 3 ; i++)
		destroi_mundo (cria_mundo ()) ;
}

// ------------------------------------------------------------ teste: falhas

#ifdef INJETA_FALHAS

static void *c_heroi (int id) { return cria_heroi (id) ; }
static void *c_base (int id) { return cria_base (id) ; }
static void *c_missao (int id) { return cria_missao (id) ; }
static void *c_invasao (int id) { return cria_invasao (id) ; }

static void l_heroi (void *p) { struct heroi *h = p ; cjto_destroi (h->hab) ; free (h) ; }
static void l_base (void *p) { struct base *b = p ; cjto_destroi (b->presentes) ; fila_destroi (b->espera) ; free (b) ; }
static void l_missao (void *p) { struct missao *m = p ; cjto_destroi (m->hab) ; free (m) ; }
static void l_invasao (void *p) { free (p) ; }

// faz o n-esimo malloc de um construtor falhar; ele deve devolver NULL
// sem deixar nada alocado para tras (o valgrind confere os vazamentos)
static void testa_falha_ctor (const char *nome, void *(*cria) (int), void (*libera) (void *))
{
	char msg[160] ;

	printf ("falha de alocacao: %s\n", nome) ;
	limite = 100 ;
	for (int n = 1 ; n <= 12 ; n++)
	{
		armar (n) ;
		void *r = cria (0) ;
		desarmar () ;
		if (disparou)
		{
			snprintf (msg, sizeof (msg), "%s deve devolver NULL se o malloc/calloc #%d falhar", nome, n) ;
			CHECA (r == NULL, msg) ;
		}
		else
		{
			// n passou do total de alocacoes: execucao normal
			CHECA (r != NULL, "construtor falhou sem injecao de falha") ;
			if (r)
				libera (r) ;
			break ;
		}
	}
	limite = 3 ;
}

static void testa_falha_mundo ()
{
	int n = 1, injetadas = 0, erradas = 0 ;

	printf ("falha de alocacao: cria_mundo\n") ;
	while (1)
	{
		armar (n) ;
		struct mundo *w = cria_mundo () ;
		desarmar () ;
		if (!disparou)
		{
			CHECA (w != NULL, "cria_mundo falhou sem injecao de falha") ;
			destroi_mundo (w) ;
			printf ("  cria_mundo faz %d alocacoes; testadas %d falhas\n", contagem, injetadas) ;
			break ;
		}
		injetadas++ ;
		if (w)
		{
			erradas++ ;
			CHECAI (0, "cria_mundo deveria devolver NULL quando uma alocacao falha (indice = numero do malloc que falhou)", n) ;
			destroi_mundo (w) ;
		}
		n += (n < 60) ? 1 : 37 ;
	}
	if (erradas)
		printf ("  cria_mundo devolveu mundo valido em %d de %d falhas injetadas\n", erradas, injetadas) ;
}

#endif

// ---------------------------------------------------------------- main

int main ()
{
	testa_aleat () ;
	testa_macros () ;
	testa_mundo () ;
	testa_destroi_nulo () ;
	testa_repeticao () ;

#ifdef INJETA_FALHAS
	testa_falha_ctor ("cria_heroi", c_heroi, l_heroi) ;
	testa_falha_ctor ("cria_base", c_base, l_base) ;
	testa_falha_ctor ("cria_missao", c_missao, l_missao) ;
	testa_falha_ctor ("cria_invasao", c_invasao, l_invasao) ;
	testa_falha_mundo () ;
#else
	printf ("(testes de falha de alocacao desligados: compile com -DINJETA_FALHAS e -Wl,--wrap=malloc -Wl,--wrap=calloc)\n") ;
#endif

	for (int i = 0 ; i < MAX_LINHAS ; i++)
		if (ocultas[i])
			printf ("  (linha %d: mais %d falhas iguais nao mostradas)\n", i, ocultas[i]) ;

	printf ("\n%d verificacoes, %d falhas\n", total, falhas) ;
	if (falhas == 0)
		printf ("TUDO OK (agora rode no valgrind para checar a memoria)\n") ;
	return falhas != 0 ;
}