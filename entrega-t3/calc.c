#include "str.h"
#include "lista.h"
#include "calc.h"
#include "dicionario.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// Area das funções auxiliares


static bool caractereNaoUsado(unichar trecho){
    if (trecho == '\0' || trecho == '\t' || trecho == '\n' 
        || trecho == ' ' || trecho == '\0') return true;
    return false;
}

static bool numero(Str txt, int pos){
    unichar conversao = s_ch(txt, pos);
    if (conversao >= '0' && conversao <= '9') return true;
    return false;
}

static bool ponto(Str txt, int pos){
    unichar conversao = s_ch(txt, pos);
    if (conversao == '.') return true;
    return false;
}

static bool SinaisBool(Str txt, int pos){
    if (txt == NULL) return false;
    unichar conversao = s_ch(txt, pos);
    if(conversao == '+' || conversao == '-' || conversao == '*' || conversao == '=' ||
    conversao == '/' || conversao == '(' || conversao == ')' ||conversao == '^'){
        return true;
    }   
    return false;
}

static bool Hashtag(Str txt, int pos){
    if (txt == NULL) return false;
    if(s_ch(txt, pos) == '#')return true;
    return false;
}

static bool cifrao(Str txt, int pos){
    if (txt == NULL) return false;
    if(s_ch(txt, pos) == '$')return true;
    return false;
}

static bool variavel(Str txt, int pos){
    if (txt == NULL) return false;
    unichar conversao = s_ch(txt, pos);
    if(conversao >= 'a' && conversao <= 'z' || conversao >= 'A' && conversao <= 'Z'){
        return true;
    }
    return false;   
}

static void acaoNum(dado_t dadoNum, Dicionário dic, Lista operandos, double *num){
    if (variavel(dadoNum, 0) || Hashtag(dadoNum, 0) || cifrao(dadoNum, 0)){
        valor_t ValorUm = dic_busca(dic, dadoNum);
        if (ValorUm == VALOR_NÃO_EXISTE){
            l_empilha(operandos, s_cria("#ERRO: variavel não existe"));
            return;
        }
        *num = s_número(ValorUm);
    } else{
        *num = s_número(dadoNum);
        return;
    }
}

static void atribuicao(dado_t DadoNumeroEsquerda, Lista operandos, Dicionário dic,
                       dado_t DadoNumeroDireita, double numeroDaDireita){
    if(!variavel(DadoNumeroEsquerda,0) && !Hashtag(DadoNumeroEsquerda,0) && !cifrao(DadoNumeroEsquerda,0)){
        l_empilha(operandos, s_cria("#ERRO: atribuição de número para número"));
        return;
    }
    dic_insere(dic, DadoNumeroEsquerda, DadoNumeroDireita);
    l_empilha(operandos, s_cria_número(numeroDaDireita));
    return;
}

static void empilhaResultado(char sinal, double numeroDaEsquerda, double numeroDaDireita,
                             Lista operandos){
    double resultado = 0;
    switch (sinal){
    case '+':
        resultado = numeroDaEsquerda + numeroDaDireita;
        break;
    case '-':
        resultado = numeroDaEsquerda - numeroDaDireita;
        break;
    case '*':
        resultado = numeroDaEsquerda * numeroDaDireita;
        break;
    case '/':
        resultado = numeroDaEsquerda / numeroDaDireita;
        break;
    case '^':
        resultado = pow(numeroDaEsquerda, numeroDaDireita);
        break;
    default:
        l_empilha(operandos, s_cria("#ERRO: não foi possivel resolver"));
        return;
    }
    l_empilha(operandos, s_cria_número(resultado));
}

static void conta(Lista operadores, Lista operandos, Dicionário dic){
    dado_t DadoNumeroDireita = l_desempilha(operandos);
    double numeroDaDireita = 0;
    dado_t DadoNumeroEsquerda = l_desempilha(operandos);
    double numeroDaEsquerda = 0;
    dado_t StrSinal = l_desempilha(operadores);
    char sinal = s_ch(StrSinal, 0);
    if(DadoNumeroDireita == NULL || DadoNumeroEsquerda == NULL || StrSinal == NULL){
        DadoNumeroEsquerda = s_cria("#ERRO: não conseguiu encontrar expressão válida");
        l_empilha(operandos, DadoNumeroEsquerda);
        return;
    }   
    acaoNum(DadoNumeroDireita, dic, operandos, &numeroDaDireita);
    if (sinal == '='){
        atribuicao(DadoNumeroEsquerda, operandos, dic, DadoNumeroDireita, numeroDaDireita);
        return;
    } 
    acaoNum(DadoNumeroEsquerda, dic, operandos, &numeroDaEsquerda);
    empilhaResultado(sinal, numeroDaEsquerda, numeroDaDireita, operandos);
}

static int prioridade(char op){
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    if (op == '=') return 0;
    return -1;
}

static void OperadorAcao(Lista retorno, Str txt, int *pos){
    l_insere_fim(retorno, s_cria_substring(txt, *pos, 1));
    (*pos)++;
}

static void NumeroAcao(Lista retorno, Str txt, int *pos, int *tamInicio){
    *tamInicio = *pos;
    while (*pos < s_tam(txt) && (numero(txt, *pos) || ponto(txt, *pos))) (*pos)++;
    l_insere_fim(retorno, s_cria_substring(txt, *tamInicio, *pos - *tamInicio));
}

static void VariavelAcao(Lista retorno, Str txt, int *pos, int *tamInicio){
    *tamInicio = *pos;
    if(Hashtag(txt, *pos) || cifrao(txt, *pos)) (*pos)++;
    while ((*pos < s_tam(txt) && (variavel(txt, *pos) || numero (txt, *pos)))
            && !Hashtag(txt, *pos) && !cifrao(txt, *pos)) (*pos)++;
    l_insere_fim(retorno, s_cria_substring(txt, *tamInicio, *pos - *tamInicio));
}


//funções principais da Calculadora

static void EmpilhaNumero(dado_t dado, Lista operandos){
    if (numero(dado, 0)) l_empilha(operandos, dado);
}

static void EmpilhaVariavel(dado_t dado, Lista operandos){
    if (Hashtag(dado,0) || variavel(dado, 0) || cifrao(dado, 0))l_empilha(operandos, dado);
}

static bool ordemAltera(char charAtual, Lista sinais){
    return (charAtual == '=' || charAtual == '^') && charAtual == s_ch(l_topo(sinais), 0);
}

static void Calcula(dado_t dado, Lista numeros, Lista sinais, Dicionário dic){
    if (l_vazia(sinais)) l_empilha(sinais, dado);
    else{
        if (!l_vazia(numeros)){
            char CharAtual = s_ch(dado, 0);
            if (CharAtual == '(') l_empilha(sinais, dado);
            else if (CharAtual == ')'){
                while (!l_vazia(sinais) && s_ch(l_topo(sinais), 0) != '('){
                    conta(sinais,numeros, dic);
                }
                if (!l_vazia(sinais) && s_ch(l_topo(sinais), 0) == '(') l_desempilha(sinais);
                else l_empilha(sinais, s_cria("#ERRO"));
            } else {
                while(!l_vazia(sinais) && s_ch(l_topo(sinais), 0) != '(' 
                        && prioridade(s_ch(l_topo(sinais), 0)) >= prioridade(CharAtual) 
                        && !ordemAltera(CharAtual, sinais)){
                    conta(sinais, numeros, dic);
                }
                l_empilha(sinais, dado);
            }    
        }
    }
}

// funções principais do arquivo

Str calculadora(Str expressão, Dicionário dic){
    Lista operandos = l_cria();
    Lista operadores = l_cria();
    Lista tokens = tokeniza(expressão);
    while (!l_vazia(tokens)){
        dado_t dado = l_remove_inicio(tokens);
        EmpilhaNumero(dado, operandos);
        EmpilhaVariavel(dado, operandos);
        if (SinaisBool(dado,0)) Calcula(dado, operandos, operadores, dic);
    }
    Str resultado;
    if (l_vazia(operadores)) resultado = s_cria("#ERRO: operadores não suficientes");
    else if(l_vazia(operandos)) resultado = s_cria("#ERRO: operandos não suficientes");
    else{
        while(!l_vazia(operadores)) conta(operadores, operandos, dic);
        resultado = l_desempilha(operandos);
    }
    l_destroi(operandos);
    l_destroi(operadores);
    l_destroi(tokens);
    return resultado;
}


Lista tokeniza(Str txt){
    Lista retorno = l_cria();
    int pos = 0;
    int tamInicio = 0;
    while (pos < s_tam(txt)){
        if (!caractereNaoUsado(s_ch(txt, pos))){
            if (numero(txt,pos)) NumeroAcao(retorno, txt, &pos, &tamInicio);
            else if (variavel(txt, pos) || Hashtag(txt, pos) || cifrao(txt, pos)) VariavelAcao(retorno, txt, &pos, &tamInicio);
            else OperadorAcao(retorno, txt, &pos);
        }else pos++;
    }
    return retorno;
}