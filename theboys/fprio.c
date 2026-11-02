// TAD Fila de prioridades (FPRIO) genérica
// Carlos Maziero, DINF/UFPR, Out 2024
// Marcos Castilho, inclui as structs no arquivo.c, Out 2025
// Implementação com fila encadeada simples

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fprio.h"

//Descreve um nodo da fila de prioridades
struct fpnodo_t
{
  void *item ;          //Item associado ao nodo
  int   tipo ;          //Tipo do item
  int   prio ;          //Prioridade do item
  struct fpnodo_t *prox;    //Próximo nodo
};

//Descreve uma fila de prioridades
struct fprio_t
{
  struct fpnodo_t *prim ;   //Primeiro nodo da fila
  int num ;         //Número de itens na fila
} ;

//Cria uma fila vazia.
//Retorno: ponteiro para a fila criada ou NULL se erro.
struct fprio_t *fprio_cria (){
  struct fprio_t *nova_fila = malloc (sizeof(struct fprio_t)); //Aloca memória para a fila

  if (!nova_fila) //Se tiver problema na alocação da fila, retorna NULL
    return NULL;
  
  memset (nova_fila, 0, sizeof(struct fprio_t)); //Preenche a fila com 0

  return nova_fila;
}

//Libera todas as estruturas de dados da fila, inclusive os itens.
//Retorno: NULL.
struct fprio_t *fprio_destroi (struct fprio_t *f){
  if (!f) //Se não existir a fila, retorna NULL
    return NULL;
  
  struct fpnodo_t *aux; //Cria um auxiliar
  for (int i = 0; i < f->num; i++){ //Percorre a fila até tamanho
    aux = f->prim; //Auxiliar recebe o ponteiro com o início da fila
    f->prim = aux->prox; //O ponteiro de início da fila recebe o próximo na fila
    free(aux->item);
    free (aux); //Desaloca o auxiliar
  }

  free (f); //Desaloca a fila
  return NULL;
}

//Insere o item na fila, mantendo-a ordenada por prioridades crescentes.
//Itens com a mesma prioridade devem respeitar a politica FIFO (retirar
//na ordem em que inseriu).
//Inserir duas vezes o mesmo item (o mesmo ponteiro) é um erro.
//Retorno: número de itens na fila após a operação ou -1 se erro.
int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio){
  if (!f || !item) //Se não existir a lista ou o item, retorna -1
    return -1;

  struct fpnodo_t *atual;
  struct fpnodo_t *novo = malloc (sizeof (struct fpnodo_t)); //Aloca memória pro item inserido
  
  if (!novo){ //Testa se a alocação funcionou
    free(novo);
    return -1;
  }

  //Insere o item no item e inicializa tipo e prio
  novo->item = item;
  novo->tipo = tipo;
  novo->prio = prio;
  novo->prox = NULL;


  //Verifica se o mesmo ponteiro já está na fila.
  atual = f->prim;
  while (atual != NULL){
    if (atual->item == item){
      free(novo);
      return -1;
    }

    atual = atual->prox;
  }

  if (f->prim == NULL || prio < f->prim->prio){ //Caso seja o primeiro ou a prioridade seja menor que a primeira
    novo->prox = f->prim;
    f->prim = novo;
  }
  else{ //Caso esteja no meio ou fim
    atual = f->prim;

    while (atual->prox != NULL && atual->prox->prio <= prio) //Testa a prioridade
      atual = atual->prox;
    
    novo->prox = atual->prox;
    atual->prox = novo;
  }

  f->num++; //Aumenta o tamanho da fila

  return f->num;
}

//Retira o primeiro item da fila e o devolve; o tipo e a prioridade
//do item são devolvidos nos parâmetros "tipo" e "prio".
//Retorno: ponteiro para o item retirado ou NULL se fila vazia ou erro.
void *fprio_retira (struct fprio_t *f, int *tipo, int *prio){
  if (!f || f->prim == NULL || !tipo || !prio) //Se não existir a fila ou não tiver elementos, retorna NULL
    return NULL;

  //Retorna o tipo e prio do removido
  *tipo = f->prim->tipo;
  *prio = f->prim->prio;

  //Coloca o que vai ser removido em outra variável
  struct fpnodo_t *removido;
  removido = f->prim;

  //Coloca o que estava de próximo como primeiro e coloca o proximo do removido de NULL
  f->prim = f->prim->prox;
  removido->prox = NULL;

  void *item_removido = removido->item;

  f->num--;
  free(removido);

  return item_removido;
}

//Informa o número de itens na fila.
//Retorno: N >= 0 ou -1 se erro.
int fprio_tamanho (struct fprio_t *f){
  if (!f || f->num < 0) //Se não existir a fila ou o tamanho for menor que 0, retorna -1
    return -1;

  return f->num;
}

//Imprime o conteúdo da fila no formato "(tipo prio) (tipo prio) ..."
//Para cada item deve ser impresso seu tipo e sua prioridade, com um
//espaço entre valores, sem espaços antes ou depois e sem nova linha.
void fprio_imprime (struct fprio_t *f){
  if (!f) //Se não existir a fila, retorna
    return;

  struct fpnodo_t *nodo = f->prim;

  while (nodo){
    printf("(%d %d)", nodo->tipo, nodo->prio); //Imprime o valor

    if (nodo->prox) //Imprime espaço se tiver próximo
      printf(" ");

    nodo = nodo->prox;
  }
}