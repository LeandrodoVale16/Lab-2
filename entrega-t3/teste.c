#include <stdio.h>
#include <string.h>
#include "calc.h"
#include "dicionario.h"

// Compara duas chaves do tipo Str usando a função nativa s_igual
bool igual_str(chave_t a, chave_t b) {
    return s_igual(a, b);
}

// Extrai a string C nativa para usar o strcmp e verificar quem é menor
bool menor_str(chave_t a, chave_t b) {
    char *sa = s_strc(a);
    char *sb = s_strc(b);
    bool resultado = strcmp(sa, sb) < 0;
    free(sa);
    free(sb);
    return resultado;
}

int main(){
    Dicionário dic = dic_cria(menor_str, igual_str);
    
    // Primeira chamada: Atribuição
    Str conta1 = s_cria("va = 2");
    Str B1 = calculadora(conta1, dic);
    s_imprime(B1);
    
    // Segunda chamada: Utilização da variável
    Str conta2 = s_cria(" vb = 5");
    Str B2 = calculadora(conta2, dic);
    s_imprime(B2);

    Str conta3 = s_cria("va * vb");
    Str B3 = calculadora(conta3, dic);
    s_imprime(B3);
    
    // Libertar a memória de todas as variáveis
    dic_destrói(dic);
    s_destroi(conta1);
    s_destroi(B1);
    s_destroi(conta2);
    s_destroi(B2);
    s_destroi(conta3);
    s_destroi(B3);
    
    return 0;
}