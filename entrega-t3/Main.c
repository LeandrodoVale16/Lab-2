#include "dicionario.h"
#include "str.h"
#include "calc.h"
#include <stdio.h>


bool igual(chave_t a, chave_t b){
    Str stringA = a;
    Str stringB = b;
    return s_igual(stringA, stringB);
}

bool menor(chave_t a, chave_t b){
    Str stringA = a;
    Str stringB = b;
    int tam = 0;
    if(s_tam(stringA) >= s_tam(stringB)) tam = s_tam(stringB);
    else tam = s_tam(stringA);
    for (int i = 0; i < tam; i++){
        if ((s_ch(stringA, i) < s_ch(stringB,i))) return true;
        else if ((s_ch(stringA, i) > s_ch(stringB, i))) return false;        
    }
    return s_tam(stringA) < s_tam(stringB);
}

int main(){
    Dicionário dic = dic_cria(menor, igual);
    printf("\n-----CALCULADORA-----\n");
    printf("Digite \"sair\" para sair\n\n");
    Str palavraChave = s_cria("sair\n");
    while(true){
        char texto[100];
        printf("digite a expressao: ");
        fgets(texto, sizeof(texto), stdin);
        Str expressao = s_cria(texto);
        if(s_igual(expressao, palavraChave)) break;
        printf("resultado: ");
        Str resultado = calculadora(expressao, dic);
        s_imprime(resultado);
        s_destroi(expressao);
        s_destroi(resultado);
    }
    dic_destrói(dic);
}