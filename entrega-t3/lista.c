#include "str.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct no{
    dado_t dados;
    struct no *prox;
    struct no *ant;
}Node;

typedef struct lista{
    Node *sentinela;
    int tam;
}*Lista;

// cria e inicializa uma lista vazia
Lista l_cria(){
    Lista l = malloc(sizeof(struct lista));
    if (l == NULL) return NULL;
    l->sentinela = malloc(sizeof(Node));
    l->sentinela->prox = l->sentinela;
    l->sentinela->ant = l->sentinela;
    l->sentinela->dados = NULL;
    l->tam = 0;
    return l;
}

// cria uma lista contendo substrings de s
// as substrings são separadas por quaisquer caractere de sep
// os caracteres de sep não aparecem nas substrings
// exemplos:
//   "a,ba,ca, te", ", " -> ["a" "ba" "ca" "te"]
//   "aba \ncate\n", "\n" -> ["aba " "cate"]
Lista l_cria_separando(Str s, Str sep){
    Lista temp = l_cria();
    int pos = s_busca_nc(s, 0, sep);
    int fim;
    while (pos != -1 && pos < s_tam(s)){
        fim = s_busca_c(s, pos, sep);
        if(fim == -1) fim = s_tam(s);
        dado_t d = malloc(sizeof(dado_t));
        d = s_cria_substring(s, pos, fim - pos);
        l_insere_fim(temp, d);
        pos = s_busca_nc(s, fim, sep);
    }
    return temp;
}

// libera a memória ocupada por uma lista
void l_destroi(Lista l){
    Node *p = l->sentinela->prox;
    while(p != l->sentinela){
        Node *prox = p->prox;
        free(p);
        p = prox;
    }
    free(l->sentinela);
    free(l);
}

// retorna o número de elementos na lista
int l_tam(Lista l){
    return l->tam;
}

// retorna true se a lista tiver cheia
bool l_cheia(Lista l){
    return false;
}

// retorna true se a lista tiver vazia
bool l_vazia(Lista l){
    if (l == NULL || l->tam == 0){
        return true;
    }
    return false;
}

// imprime os dados que estão na lista
void l_imprime(Lista l){
    Node *p = l->sentinela->prox;
    while(p != l->sentinela){
        if (p->dados != NULL){
            s_imprime(p->dados);
        }
        p = p->prox;
    }
}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d){
    Node *p = malloc(sizeof(Node));
    p->dados = d;
    if (l->sentinela->prox == NULL && l->sentinela->ant == NULL)
    {
        l->sentinela->prox = p;
        l->sentinela->ant = p;
        p->ant = l->sentinela;
        p->prox = l->sentinela;
    } else{
        l->sentinela->prox->ant = p;
        p->prox = l->sentinela->prox;
        l->sentinela->prox = p;
        p->ant = l->sentinela;
        l->tam++;
    }
}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d){
    Node *p = malloc(sizeof(Node));
    p->dados = d;
    if (l->sentinela->prox == NULL && l->sentinela->ant == NULL)
    {
        l->sentinela->ant = p;
        l->sentinela->prox = p;
        p->ant = l->sentinela;
        p->prox = l->sentinela;
    } else {
        l->sentinela->ant->prox = p;
        p->prox = l->sentinela;
        p->ant = l->sentinela->ant;
        l->sentinela->ant = p;
        l->tam++;
    }
}

// insere o dado d na lista l, de forma que ele fique na posição p
// a primeira posição é 0
void l_insere_pos(Lista l, dado_t d, int p){
    Node *temp = malloc(sizeof(Node));
    temp->dados = d;
    Node *loop = l->sentinela;
    loop = loop->prox;
    if (l->sentinela->prox == NULL && l->sentinela->ant == NULL) p = 0;
    for (int i = 0; i < p; i++){
        loop = loop->prox;
    }
    loop->ant->prox = temp;
    temp->ant = loop->ant;
    temp->prox = loop;
    loop->ant = temp;
    l->tam++;
}

// retorna o dado no início da lista
dado_t l_dado_inicio(Lista l){
    return l->sentinela->prox->dados;
}

// retorna o dado no final da lista
dado_t l_dado_fim(Lista l){
    return l->sentinela->ant->dados;
}

// retorna o dado na posição pos da lista
dado_t l_dado_pos(Lista l, int pos){
    Node *temp = l->sentinela;
    temp = temp->prox;
    int k = 0;
    while(temp->dados != NULL){
        if(k == pos) return temp->dados;
        temp = temp->prox;
        k++;
    }
    dado_t d = NULL;
    return d;
}

// remove e retorna o dado no início da lista
dado_t l_remove_inicio(Lista l){
    Node *remover = l->sentinela->prox;
    if (l_vazia(l) == true){
        dado_t d = NULL;
        return d;
    }
    dado_t temp = remover->dados;
    l->sentinela->prox = remover->prox;
    remover->prox->ant = l->sentinela;
    free(remover);
    l->tam--;
    return temp;
}

// remove e retorna o dado no final da lista
dado_t l_remove_fim(Lista l){
    dado_t temp;
    if (l_vazia(l) == true){
        temp = NULL;
        return temp;
    }
    l->sentinela->ant = l->sentinela->ant->ant;
    temp = l->sentinela->ant->prox->dados;
    free(l->sentinela->ant->prox);
    l->sentinela->ant->prox = l->sentinela;
    l->tam--;
    return temp;
}

// remove e retorna o dado na posição pos da lista
dado_t l_remove_pos(Lista l, int pos){
    Node *p = l->sentinela->prox;
    dado_t retorno;
    for (int i = 0; i < pos; i++){
        p = p->prox;
    }
    retorno = p->dados;
    p->ant->prox = p->prox;
    p->prox->ant = p->ant;
    free(p);
    l->tam--;
    return retorno;
}


// funções para usar a lista como uma fila

// l_cria, l_destroi, l_vazia

// retorna o dado que está no início da fila
dado_t l_primeiro(Lista l){
    return l->sentinela->prox->dados;
}

// insere um dado no fim da fila
void l_insere(Lista l, dado_t d){
    Node *p = malloc(sizeof(Node));
    p->dados = d;
    l->sentinela->ant->prox = p;
    p->ant = l->sentinela->ant;
    p->prox = l->sentinela;
    l->sentinela->ant = p;
    l->tam++;
}

// remove e retorna o dado que está no início da fila
dado_t l_remove(Lista l){
    dado_t temp;
    temp = l->sentinela->prox->dados;
    l->sentinela->prox = l->sentinela->prox->prox;
    free(l->sentinela->prox->ant);
    l->sentinela->prox->ant = l->sentinela;
    l->tam--;
    return temp;
}


// funções para usar a lista como uma pilha

// l_cria, l_destroi, l_vazia

// retorna o dado que está no topo da pilha
dado_t l_topo(Lista l){
    if (l_vazia(l) == true){
        dado_t d = NULL;
        return d;
    }
    return l->sentinela->ant->dados;
}

// empilha um dado no topo da pilha
void l_empilha(Lista l, dado_t d){
    Node *p = malloc(sizeof(Node));
    p->dados = d;
    if(l == NULL){
        l->sentinela->ant = p;
        p->prox = l->sentinela;
        l->sentinela->prox = p;
        p->ant = l->sentinela;
    } else{
        l->sentinela->ant->prox = p;
        p->ant = l->sentinela->ant;
        p->prox = l->sentinela;
        l->sentinela->ant = p;
        l->tam++;
    }
}

// remove e retorna o dado que está no topo da pilha
dado_t l_desempilha(Lista l){
    if (l_vazia(l) == true){
        dado_t d = NULL;
        return d;
    }
    dado_t temp = l_topo(l);
    l->sentinela->ant = l->sentinela->ant->ant;
    free(l->sentinela->ant->prox);
    l->sentinela->ant->prox = l->sentinela;
    l->tam--;
    return temp;
}