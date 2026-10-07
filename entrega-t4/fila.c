#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "fila.h"

typedef struct no{
    void *dado;
    struct no *prox;
    struct no *ant;
}*No;

struct fila{
    No no;
    int tam_do_dado;
};

static int ConcertaPosicao(int pos, Fila f){
    No aux = f->no;
    int tamFila = 0;
    while (aux != NULL){
        aux = aux->prox;
        tamFila++;
    }
    return pos + tamFila;
}

// funções que implementam as operações básicas de uma fila

// cria uma fila vazia que suporta dados do tamanho fornecido (em bytes)
Fila f_cria(int tam_do_dado){
    Fila aux = malloc(sizeof(struct fila));
    aux->no = NULL;
    aux->tam_do_dado = tam_do_dado;
    return aux;
}

// libera a memória ocupada pela fila
void f_destrói(Fila self){
    No remover;
    void *removerDado;
    while (self->no != NULL && self->no->prox != NULL){
        self->no = self->no->prox;
    }
    while (self->no != NULL){
        remover = self->no;
        removerDado = remover->dado;
        self->no = self->no->ant;
        free(removerDado);
        free(remover);
    }
    free(self);
}

// diz se a fila está vazia
bool f_tá_vazia(Fila self){
    if (self->no == NULL) return true;
    return false;
}

// remove o dado no início da fila e, se pdado não for NULL, copia o dado removido para *pdado
void f_remove(Fila self, void *pdado){
    if (self->no == NULL) return;
    No remover = self->no;
    self->no = self->no->prox;
    if (self->no != NULL) self->no->ant = NULL;
    if (pdado != NULL) memcpy(pdado, remover->dado, self->tam_do_dado);
    free(remover->dado);
    free(remover);
}

// insere o dado apontado por pdado no final da fila
void f_insere(Fila self, void *pdado){
    No aux = malloc(sizeof(struct no));
    aux->dado = malloc(self->tam_do_dado);
    aux->prox = NULL;
    memcpy(aux->dado, pdado, self->tam_do_dado);
    if (self->no == NULL){
        aux->ant = self->no;
        self->no = aux;
        return;
    }
    No temp = self->no;
    while (temp->prox != NULL){
        temp = temp->prox;
    }
    temp->prox = aux;
    aux->ant = temp;
}

// funções que implementam operações complementares, que permitem acesso
//   a todos os elementos da fila. Durante um percurso, a fila não pode ser
//   alterada.

// inicia um percurso aos elementos da fila, a partir de uma posição inicial
// se a posição for positiva, o percurso vai desde essa posição, até o fim da fila
// se a posição for negativa, o percurso vai desde essa posição, até o início
//   0 é a posição do primeiro dado (aquele que está na fila há mais tempo)
//   1 é a posição do segundo dado, etc
//   além disso,
//   -1 é a posição do último dado (o que está na fila há menos tempo)
//   -2 é a posição do penúltimo dado, etc
// cada dado do percurso será acessado por chamadas a f_próximo()
void f_inicia_percurso(Fila self, int pos_inicial){
    bool FimInicio = false;
    if (pos_inicial < 0){
        pos_inicial = ConcertaPosicao(pos_inicial, self);
        FimInicio = true;
    } 
    No temp = self->no;
    for (int i = 0; i < pos_inicial; i++){
        temp = temp->prox;
    }
    
    
}

// caso o percurso tenha terminado, retorna false
// senão, coloca o próximo dado do percurso em *pdado (se pdado não for NULL),
//   e retorna true
bool f_próximo(Fila self, void *pdado){ 
    if (self->no == NULL) return false;
    if (pdado != NULL) memcpy(pdado, self->no->dado, self->tam_do_dado);
    return true;
}