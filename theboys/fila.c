#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

//Cria uma fila vazia.
struct fila_t *fila_cria () {
	struct fila_t *nova_f = malloc (sizeof (struct fila_t)); //Aloca memória para a fila
	
  	if (!nova_f) //Se tiver problema na alocação da fila, retorna NULL
		return NULL;

  	//Inicializa os valores da fila
	nova_f->prim = NULL;
	nova_f->fim = NULL;
	nova_f->num = 0;

	return nova_f;
}

//Libera todas as estruturas de dados da fila, inclusive os itens.
struct fila_t *fila_destroi (struct fila_t *f){
  	if (!f) //Se não existir a fila, retorna NULL
    	return NULL;
  
  	struct fila_nodo_t *aux; //Cria um auxiliar
  	for (int i = 0; i < f->num; i++){ //Percorre a fila até o final
    	aux = f->prim; //Auxiliar recebe o ponteiro com o primeiro da fila
    	f->prim = aux->prox; //O ponteiro de início da fila recebe o próximo da fila
    	free (aux->item);	//Desaloca o item
    	free (aux); //Desaloca o nodo auxiliar
  	}

  	free (f); //Desaloca a fila
  	return NULL;
}

// Insere o item no final da fila.
int fila_insere (struct fila_t *f, int /*void*/ *item) {
	if (!f || !item) //Se não existir a fila ou o item, retorna -1
		return -1;

	struct fila_nodo_t *atual = f->prim;
  	struct fila_nodo_t *novo = malloc (sizeof (struct fila_nodo_t)); //Aloca memória pro item inserido
  	if (!novo) //Testa se a alocação funcionou
		return -1;

  	while (atual) { //Verifica se o mesmo ponteiro já está na fila.
		if (atual->item == item){
			free(novo);
      return -1;
    }
		atual = atual->prox;
	}

  	//Insere o item no item e inicializa o prox
	novo->item = item;
	novo->prox = NULL;

	if (!f->fim)	//Caso a fila esteja vazia
		f->prim = novo;
	else
		f->fim->prox = novo;

	f->fim = novo; //O final da fila recebe o novo
	f->num++; //Incrementa o tamanho da fila

	return f->num;
}

// Retira o primeiro item da fila e o devolve.
void *fila_retira (struct fila_t *f) {
	if (!f || !f->prim) //Se não existir a fila ou o primeiro item, retorna NULL
		return NULL;

  	//Coloca o que vai ser removido em outra variável
	struct fila_nodo_t *removido = f->prim;
	void *item_removido = removido->item;

  	//Coloca o que estava de próximo como primeiro e coloca o proximo do removido de NULL
	f->prim = removido->prox;
  	removido->prox = NULL;

	if (!f->prim) //Se a fila ficou vazia, fim recebe NULL
		f->fim = NULL;

	free (removido); //Desaloca o removido
	f->num--; //Decrementa o tamanho da fila

	return item_removido;
}

//Informa o número de itens na fila.
int fila_tamanho (struct fila_t *f){
  	if (!f) //Se não existir a fila, retorna -1
    	return -1;

  	return f->num; //Retorna o tamanho da fila
}

//Imprime o conteúdo da fila.
void fila_imprime (struct fila_t *f){
	if (!f) //Se não existir a fila, retorna
		return;

  	struct fila_nodo_t *aux = f->prim;

	while (aux){ //Imprime o item
		printf ("%p", aux->item);

		if (aux->prox) //Imprime espaço se tiver próximo
			printf (" ");

		aux = aux->prox;
	}
}