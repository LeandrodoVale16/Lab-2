// includes, constantes e declarações {{{1
#include "str.h"
#include "utf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

typedef struct str {
  char *string;
  int tamanhoCaracteres;
  int bytesAlocados;
}*Str;

// A memória para conter os bytes de uma string deve ser alocada e/ou
//   realocada conforme a necessidade, cuidando para que a quantidade
//   de memória alocada seja sempre:
//   - nula (não alocada) se a string for vazia, ou
//   - não inferior ao necessário para armazenar os bytes da codificação utf8;
//   - não inferior à alocação mínima;
//   - não superior ao triplo do número de bytes necessários
//     (exceto quando for o mínimo);
//   - uma potência de 2.

// funções auxiliares {{{1

static int tamanhoBytesStrC(char const *s){
  return strlen(s);
}

static void tamanhoAlocado(Str l){
  l->bytesAlocados = strlen(l->string);
}

static void quantidadeCaracteres(Str l){
  l->tamanhoCaracteres = strlen(l->string);
}

static void arrumaPos(int *pos, byte *texto){
  if (pos < 0){
    int tamTex = strlen(texto);
    int x = u8_conta_unichar_nos_bytes(tamTex, texto);
    if(x != -1){
      pos = pos + (x+1); //pos+tam+1 inverte a pos
    }
  }
}

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s)
{
  assert(s != NULL);
  assert(s->bytesAlocados >= s->tamanhoCaracteres);

}

//...

// operações de criação e destruição {{{1

Str s_cria(char const *strC)
{
  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  int bytesPontenciaDeDois = MIN_ALLOC;
  int x = tamanhoBytesStrC(strC);
  byte *copia;
  copia = malloc(x);
  strcpy(copia, strC);
  int verifica = u8_conta_unichar_nos_bytes(x, copia);
  if (verifica > -1){
    //da pra fazer em um while
    for(int i = 0; bytesPontenciaDeDois < x; i++){
      bytesPontenciaDeDois = bytesPontenciaDeDois * 2;
    }
    if (bytesPontenciaDeDois >= x && bytesPontenciaDeDois <= 3 * bytesPontenciaDeDois){
      s->string = malloc(bytesPontenciaDeDois);
      strcpy(s->string, strC);
      s->bytesAlocados = bytesPontenciaDeDois;
      s->tamanhoCaracteres = verifica;
    } else if (x == 0)
    {
      s->string = malloc(bytesPontenciaDeDois);
      s->bytesAlocados = bytesPontenciaDeDois;
      s->tamanhoCaracteres = verifica;
    }
  }
  free(copia);
  return s;
}

void s_destroi(Str s)
{
  s_ok(s);
  free(s->string);
  free(s);
}

Str s_cria_substring(Str_c s, int pos, int tam)
{
  Str nova = s_cria("");
  s_substring(nova, s, pos, tam);
  return nova;
}

Str s_cria_cópia(Str_c s)
{
  return s_cria_substring(s, 0, -1);
}

// Retorna uma nova string com o conteúdo do arquivo chamado nome.
// Retorna uma string vazia em caso de erro.
Str s_cria_de_arquivo(char *nome)
{
  Str s = s_cria("");
  //...
  return s;
}

// operações de acesso {{{1

int s_tam(Str_c s)
{
  s_ok(s);
  int x = strlen(s->string);
  int verifica = u8_conta_unichar_nos_bytes(x, s->string);
  if (verifica > 0){
    return verifica;
  } else {
    return 0;
  }
}

char *s_strc(Str_c s)
{
  s_ok(s);
  //...
  return NULL;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  //...
  return UNI_INV;
}


// operações de busca e comparação {{{1

bool s_igual(Str_c s, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  int x = strcmp(s->string, sb->string);
  if (x == 0){
    return true;
  } else {
    return false;
  }
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  //...
  return -1;
}


// operações de alteração {{{1

//sb vai ser a string a ser inserida
void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  int *Ppos = pos;
  byte *copia = s->string;
  arrumaPos(Ppos, copia);
  byte algumaCoisa = u8_avanca_unichar(copia, pos);
  for (int i = 0; i < tam; i++)
  {
    /* code */
  }
  

}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  s_ok(s);
  s_ok(sb);
  //...
}

void s_copia(Str s, Str_c sb)
{
  s_substring(s, sb, 0, -1);
}

void s_insere(Str s, int pos, Str_c sb)
{
  s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
  s_ok(s);
  //...
}

void s_anexa(Str s, Str_c sb)
{
  s_substitui(s, -1, 0, sb);
}

void s_anexa_c(Str s, unichar c)
{
  s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
  s_substitui(s, pos, tam, NULL);
}

void s_apara(Str s, Str_c sobras)
{
  s_ok(s);
  s_ok(sobras);
  //...
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);
  printf("%s", s->string);
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
  FILE *arquivo;
  arquivo = fopen("nome.txt", "w");
  fprintf(arquivo, "%s", s);
  fclose(arquivo);
}


// vim: foldmethod=marker shiftwidth=2

