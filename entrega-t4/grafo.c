#include "fila.h"
#include "grafo.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct aresta{
    int proxNo;
    void *valorAresta;
    struct aresta *prox;
}*Aresta;

typedef enum estado{
    NAO_VISITADO,
    EM_VISITA,
    VISITADO
} status;

typedef struct no{
    void *valorNo;
    Aresta arestaPrincipal;
    status estadoVisita;
}*No;

struct grafo{
    int NumNos;
    No *NoPrincipal;
    Aresta ArestaAtual;
    int tam_no;
    int tam_aresta;
};

// funções que implementam as operações básicas de um grafo
//
// um grafo é constituido por nós e arestas que os interligam.
// existe um valor associado a cada nó e a cada aresta. Esses valores são
//   armazenados pelo grafo, mas o grafo desconhece seu tipo, só conhece
//   seu tamanho.
// os nós são identificados por um inteiro que corresponde à ordem em que
//   são inseridos no grafo, com 0 correspondendo ao primeiro.
// a identificação de um nó é sempre um número inferior ao número de nós
//   no grafo. A remoção de um nó altera a identificação dos nós com números
//   superiores.
// as arestas são direcionadas, e são identificadas pelo seu nó de origem
//   e de destino.
// os valores das arestas e dos nós são copiados para o grafo. O acesso a
//   esses valores é por meio de ponteiros. O usuário do grafo pode alterar
//   o valor armazenado no grafo após obter o ponteiro (com g_valor_do_nó
//   por exemplo). O usuário pode também alterar o valor usando uma função
//   de alteração (como g_altera_valor_do_nó).

// cria um grafo vazio que suporta dados do tamanho fornecido (em bytes)
//   nos nós e nas arestas
Grafo g_cria(int tam_nó, int tam_aresta){
    Grafo g = malloc(sizeof(struct grafo));
    if (g == NULL) return NULL;
    g->NoPrincipal = NULL;
    g->ArestaAtual = NULL;
    g->NumNos = 0;
    g->tam_aresta = tam_aresta;
    g->tam_no = tam_nó;
    return g;
}

// libera a memória ocupada por um grafo
void g_destrói(Grafo self){
    free(self->NoPrincipal);
    free(self->ArestaAtual);
    free(self);
}

// Nós

// insere um nó no grafo, com o dado apontado por p_dado
// retorna o número do novo nó (que é o número de nós que
//   existia antes da inserção)
int g_insere_nó(Grafo self, void *p_dado){
    No node = malloc(sizeof(struct no));
    node->valorNo = malloc(self->tam_no);
    node->arestaPrincipal = NULL;
    node->estadoVisita = NAO_VISITADO;
    memcpy(node->valorNo, p_dado, self->tam_no);
    self->NoPrincipal = realloc(self->NoPrincipal, (self->NumNos + 1) * sizeof(No));
    self->NoPrincipal[self->NumNos] = node;
    self->NumNos+= 1;
    return self->NumNos - 1; 
}

static void movimentaIndice(Grafo self, int nó){
    for (int j = 0; j < self->NumNos; j++){
        if (j != nó) g_altera_valor_da_aresta(self, j, nó, NULL);
    }
    for (int i = nó; i < self->NumNos - 1; i++){
        self->NoPrincipal[i] = self->NoPrincipal[i+1];
    }
}

static void alteraProxNo(Grafo self, int nó){
    for (int i = 0; i < self->NumNos - 1; i++)
    {
        Aresta temp = self->NoPrincipal[i]->arestaPrincipal;
        while (temp != NULL){
            if (temp->proxNo > nó) temp->proxNo = temp->proxNo - 1;
            temp = temp->prox;
        }
    }
}

static void liberaArestasDoVertice(No node){
    Aresta remover = node->arestaPrincipal;
    Aresta aux;
    while(remover != NULL) {
        aux = remover->prox;
        free(remover->valorAresta);
        free(remover);
        remover = aux;
    }
}

// remove um nó do grafo e as arestas incidentes nesse nó
// a identificação dos nós remanescentes é alterada, como se esse nó nunca
//   tivesse existido
void g_remove_nó(Grafo self, int nó){
    No aux = self->NoPrincipal[nó];  
    movimentaIndice(self,nó);
    alteraProxNo(self, nó);
    self->NumNos--;
    liberaArestasDoVertice(aux);
    free(aux->valorNo);
    free(aux);
}

// altera o valor associado a um nó (copia o valor apontado por p_dado para
//   uma memória gerenciada pelo grafo)
void g_altera_valor_do_nó(Grafo self, int nó, void *p_dado){
    memcpy(self->NoPrincipal[nó]->valorNo, p_dado, self->tam_no);
}

// retorna um ponteiro para o valor associado a um nó que está na memória
//   gerenciada pelo grafo.
void *g_valor_do_nó(Grafo self, int nó){
    return self->NoPrincipal[nó]->valorNo;
}

// retorna o número de nós do grafo
int g_num_nós(Grafo self){
    return self->NumNos;
}

// Arestas

// altera o valor da aresta que interliga o nó origem ao nó destino
//   (copia de *p_dado para uma memória gerenciada pelo grafo)
// caso a aresta não exista, deve ser criada
// caso p_dado seja NULL, a aresta deve ser removida
void g_altera_valor_da_aresta(Grafo self, int nó_origem, int nó_destino, void *p_dado){
    if (nó_origem < 0 || nó_origem >= self->NumNos) return;
    if (nó_destino < 0 || nó_destino >= self->NumNos) return;
    Aresta temp = self->NoPrincipal[nó_origem]->arestaPrincipal;
    Aresta antTemp = NULL;
    while (temp != NULL){
        if (temp->proxNo == nó_destino) break;
        antTemp = temp;
        temp = temp->prox;
    }
    if (temp == NULL && p_dado == NULL){
        return;
    }
    if (p_dado == NULL && temp != NULL){
        if (antTemp == NULL) self->NoPrincipal[nó_origem]->arestaPrincipal = temp->prox;
        else antTemp->prox = temp->prox;
        free(temp->valorAresta);
        free(temp);
        return;
    }
    if (temp == NULL){
        Aresta aux = malloc(sizeof(struct aresta));
        aux->valorAresta = malloc(self->tam_aresta);
        aux->proxNo = nó_destino;
        aux->prox = NULL;
        memcpy(aux->valorAresta, p_dado, self->tam_aresta);
        if (self->NoPrincipal[nó_origem]->arestaPrincipal == NULL) self->NoPrincipal[nó_origem]->arestaPrincipal = aux;
        else antTemp->prox = aux;
    } else {
        memcpy(temp->valorAresta, p_dado, self->tam_aresta);
    }
    
}

// retorna um ponteiro para o valor associado à aresta entre nó_origem e 
//   nó_destino, localizado em memória gerenciada pelo grafo.
// retorna NULL se tal aresta não existir
void *g_valor_da_aresta(Grafo self, int nó_origem, int nó_destino){
    if (nó_origem < 0 || nó_origem >= self->NumNos) return NULL;
    if (nó_destino < 0 || nó_destino >= self->NumNos) return NULL;
    Aresta temp = self->NoPrincipal[nó_origem]->arestaPrincipal;
    while(temp != NULL){
        if (temp->proxNo == nó_destino){
            return temp->valorAresta;
        } else {
            temp = temp->prox;
        }
    }
    return NULL;
}

// inicia uma consulta a arestas que partem do nó origem
// as próximas chamadas a 'g_próxima_aresta' devem retornar os valores
//   correspondentes a cada aresta que parte desse nó
void g_arestas_que_partem(Grafo self, int nó_origem){

}

// inicia uma consulta a arestas que chegam ao nó destino
// as próximas chamadas a 'g_próxima_aresta' devem retornar os valores
//   correspondentes a cada aresta que chega nesse nó
void g_arestas_que_chegam(Grafo self, int nó_destino){

}

// retorna um ponteiro para o dado associado à próxima aresta, de acordo
//   com a última consulta iniciada por 'g_arestas_que_partem' ou 
//   'g_arestas_que_chegam'
// o valor do nó vizinho ao nó da consulta deve ser colocado em '*p_nó_vizinho'
//   (se não for NULL)
// caso não exista mais aresta que satisfaça a consulta, retorna NULL.
void *g_próxima_aresta(Grafo self, int *p_nó_vizinho){
    if (self->ArestaAtual == NULL) return NULL;
    if (p_nó_vizinho != NULL) *p_nó_vizinho = self->ArestaAtual->prox->proxNo;
    return self->ArestaAtual->valorAresta;   
}

// Algoritmos

// ATENÇÃO: aresta contem um ponteiro para o proximo que parte de um nó expecifico

static bool verificaLinha(Grafo self, int nó){
    if(self->NoPrincipal[nó]->estadoVisita == EM_VISITA) return true;
    if(self->NoPrincipal[nó]->estadoVisita == VISITADO) return false;
    self->NoPrincipal[nó]->estadoVisita = EM_VISITA;
    Aresta aux = self->NoPrincipal[nó]->arestaPrincipal;
    while(aux != NULL){
        if(verificaLinha(self,aux->proxNo)) return true;
        aux = aux->prox;
    }
    self->NoPrincipal[nó]->estadoVisita = VISITADO;
    return false;
}

// retorna true se grafo é cíclico, false caso contrário
bool g_tem_ciclo(Grafo self){  
    for (int j = 0; j < self->NumNos; j++){
        self->NoPrincipal[j]->estadoVisita = NAO_VISITADO;
    }
    for (int i = 0; i < self->NumNos; i++){
        if (verificaLinha(self, i)) return true;
    }
    return false;
}

// retorna uma fila de inteiros contendo os números dos nós do grafo em
//   uma ordem em que, se o nó 'a' antecede 'b', não existe uma aresta
//   de 'b' para 'a' no grafo
// deve retornar uma fila vazia caso tal ordem não exista
// quem chama esta função é responsável por destruir a fila.
Fila g_ordem_topológica(Grafo self){
    
}