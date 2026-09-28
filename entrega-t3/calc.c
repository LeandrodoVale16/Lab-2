#include "str.h"
#include "lista.h"
#include "calc.h"
#include "dicionario.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

static bool caractereNaoUsado(unichar trecho){
    if (trecho == '\0' || trecho == '\t' || trecho == '\n' || trecho == ' ' || trecho == '\0') return true;
    return false;
}

static bool numero(Str txt, int pos){
    unichar conversao = s_ch(txt, pos);
    if (conversao >= '0' && conversao <= '9') return true;
    return false;
}

static bool OperadoresBool(Str txt, int pos){
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

static bool variavel(Str txt, int pos){
    if (txt == NULL) return false;
    unichar conversao = s_ch(txt, pos);
    if(conversao >= 'a' && conversao <= 'z' || conversao >= 'A' && conversao <= 'Z'){
        return true;
    }
    return false;   
}

static void conta(Lista operadores, Lista operandos, Dicionário dic){
    dado_t DadoNumero = l_desempilha(operandos);
    double Number;
    dado_t DadoDois = l_desempilha(operandos);
    double NumberDois;
    dado_t StrSinal = l_desempilha(operadores);
    char sinal = s_ch(StrSinal, 0);
    if(DadoNumero == NULL || DadoDois == NULL || StrSinal == NULL){
        DadoDois = s_cria("#ERRO");
        l_empilha(operandos, DadoDois);
        return;
    }   
    if (numero(DadoNumero, 0) == true){
        Number = s_número(DadoNumero);
    } else if (variavel(DadoNumero, 0) == true || Hashtag(DadoNumero,0) == true){
        valor_t ValorUm = dic_busca(dic, DadoNumero);
        if (ValorUm == VALOR_NÃO_EXISTE){
            l_empilha(operandos, s_cria("#ERRO"));
            return;
        }
        Number = s_número(ValorUm);
    } else{
        l_empilha(operandos, s_cria("#ERRO"));
        return;
    }
    if (sinal == '='){
        if(variavel(DadoDois,0) == false  && Hashtag(DadoDois,0) == false){
            l_empilha(operandos, s_cria("#ERRO"));
            return;
        }
        dic_insere(dic, DadoDois, DadoNumero);
        l_empilha(operandos, s_cria_número(Number));
        return;
    }
    if (numero(DadoDois, 0) == true){
        NumberDois = s_número(DadoDois);
    } else if (variavel(DadoDois, 0) == true || Hashtag(DadoDois,0) == true){
        valor_t ValorDois = dic_busca(dic, DadoDois);
        if (ValorDois == VALOR_NÃO_EXISTE){
            l_empilha(operandos, s_cria("#ERRO"));
            return;
        }
        NumberDois = s_número(ValorDois);
    } else {
        l_empilha(operandos, s_cria("#ERRO"));
        return;
    }
    double resultado = 0;
    switch (sinal){
    case '+':
        resultado = NumberDois + Number;
        break;
    case '-':
        resultado = NumberDois - Number;
        break;
    case '*':
        resultado = NumberDois * Number;
        break;
    case '/':
        resultado = NumberDois / Number;
        break;
    case '^':
        resultado = pow(NumberDois, Number);
        break;
    default:
        l_empilha(operandos, s_cria("#ERRO"));
        return;
    break;
    }
    l_empilha(operandos, s_cria_número(resultado));
}

static int prioridade(char op){
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    if (op == '=') return 0;
    return -1;
}

//funções principais da Calculadora

static void EmpilhaNumero(dado_t dado, Lista operandos){
    if (numero(dado, 0) == true){
        l_empilha(operandos, dado);
    }
}

static void EmpilhaVariavel(dado_t dado, Lista operandos){
    if (Hashtag(dado,0) == true){
        
        l_empilha(operandos, dado);
    }
    if (variavel(dado, 0) == true){
        l_empilha(operandos, dado);
    }
}

static void Calcula(dado_t dado, Lista operandos, Lista operadores, Dicionário dic){
    if (OperadoresBool(dado,0) == true){
        if (l_vazia(operadores) == true){
            l_empilha(operadores, dado);
        } else{
            char CharAtual = s_ch(dado, 0);
            if (l_vazia(operandos) == false){
                dado_t DadoDois = l_topo(operandos);
                if (l_vazia(operandos) == false){
                    char CharAtual = s_ch(dado, 0);
                    if (CharAtual == '('){
                        l_empilha(operadores, dado);
                    } else if (CharAtual == ')'){
                        while (l_vazia(operadores) == false && s_ch(l_topo(operadores), 0) != '('){
                            conta(operadores,operandos, dic);
                        }
                        if (l_vazia(operadores) == false && s_ch(l_topo(operadores), 0) == '(') l_desempilha(operadores);
                    } else {
                        while(l_vazia(operadores) == false && s_ch(l_topo(operadores), 0) != '(' 
                        && prioridade(s_ch(l_topo(operadores), 0)) >= prioridade(CharAtual)){
                            conta(operadores, operandos, dic);
                        }
                        l_empilha(operadores, dado);
                    }    
                }
            }
        }
    }
}

// calculadora 

Str calculadora(Str expressão, Dicionário dic){
    Lista operandos = l_cria();
    Lista operadores = l_cria();
    Lista tokens = tokeniza(expressão);
    while (l_vazia(tokens) == false){
        dado_t dado = l_remove_inicio(tokens);
        EmpilhaNumero(dado, operandos);
        EmpilhaVariavel(dado, operandos);
        Calcula(dado, operandos, operadores, dic);
    }
    Str resultado;
    if(l_vazia(operandos) == true){
        resultado = s_cria("#ERRO");
    } else{
        while(l_vazia(operadores) == false){
            conta(operadores, operandos, dic);
        }
        resultado = l_desempilha(operandos);
    }
    return resultado;
}


Lista tokeniza(Str txt){
    Lista retorno = l_cria();
    int pos = 0;
    int tamInicio = 0;
    while (pos < s_tam(txt)){
        if (caractereNaoUsado(s_ch(txt, pos)) == false){
            if (numero(txt,pos) == true){
                tamInicio = pos;
                while (pos < s_tam(txt) && numero(txt,pos) == true){
                    pos++;
                }
                l_insere_fim(retorno, s_cria_substring(txt, tamInicio, pos - tamInicio));
            } else if (OperadoresBool(txt, pos) == true){
                l_insere_fim(retorno, s_cria_substring(txt, pos, 1));
                pos++;
            }else if (variavel(txt, pos) == true || Hashtag(txt, pos) == true){
                tamInicio = pos;
                if(Hashtag(txt, pos) == true) pos++;
                while ((pos < s_tam(txt) && (variavel(txt, pos) == true || numero (txt, pos) == true))
                && Hashtag(txt,pos) == false){
                    pos++;
                }
                l_insere_fim(retorno, s_cria_substring(txt, tamInicio, pos - tamInicio));
            } else {
                l_insere_fim(retorno, s_cria_substring(txt, pos, 1));  
                pos++; 
            }
        } else {
            pos++;
        }
    }
    return retorno;
}