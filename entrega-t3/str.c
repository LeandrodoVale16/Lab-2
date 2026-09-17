// includes, constantes e declarações {{{1
#include "str.h"
#include "utf8.h"
#include "lista.h"

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

static void arrumaPos(int *pos, byte *texto){
  if (*pos < 0){
    int tamTex = strlen(texto);
    int x = u8_conta_unichar_nos_bytes(tamTex, texto);
    if (x != -1){
      if (*pos > x){
        *pos = x;
      }else if(*pos < 0 && *pos + x < 0){
        *pos = 0;
      } else if(*pos < 0){
        *pos = *pos + (x + 1);
      }
    }
  }
}

static void arrumaTam(int *tam, int *pos, byte *texto){
  if (*tam < 0){
    int tamTex = strlen(texto);
    int x = u8_conta_unichar_nos_bytes(tamTex, texto);
    *tam = x - *pos;
  }
}

static int alocacao(int x){
  int bytesPontenciaDeDois = MIN_ALLOC;
  while(bytesPontenciaDeDois <= x + 1 && bytesPontenciaDeDois <= 3 * x){ 
    bytesPontenciaDeDois = bytesPontenciaDeDois * 2;
  }
  return bytesPontenciaDeDois;
}

static bool uniIgual(unichar c, Str_c sb){
  unichar sbUni;
  int bytesSb = strlen(sb->string);
  byte *lugar = sb->string;
  while (bytesSb > 0){
    int x = u8_unichar_nos_bytes(strlen(sb->string), lugar, &sbUni);
  if (c == sbUni){
    return true;
  } else{
    bytesSb-= x;
    lugar+= x;
  }
  }
  return false;
}

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s) //ta com problema
{
  assert(s != NULL);
  assert(s->bytesAlocados >= s->tamanhoCaracteres);
  assert(s->tamanhoCaracteres >= 0  && s != NULL);
}

//...

// operações de criação e destruição {{{1

Str s_cria(char const *strC) //tem problema
{
  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  if (strC == NULL || strlen(strC) == 0){
    s->string = NULL;
    s->tamanhoCaracteres = 0;
    s->bytesAlocados = 0;
    return s;
  }
  int x = tamanhoBytesStrC(strC);
  byte *copia;
  copia = malloc(x + 1);
  strcpy(copia, strC);
  int verifica = u8_conta_unichar_nos_bytes(x, copia);
  free(copia);
  int k = alocacao(x);
  s->string = malloc(k);
  s->bytesAlocados = k;
  if (verifica > -1){
    strcpy(s->string, strC);
    s->tamanhoCaracteres = verifica;
  } else {
    s->string = NULL;
    s->tamanhoCaracteres = 0;
  }
  return s;
}

Str s_cria_número(double num)
{
  char temp[100];
  sprintf(temp, "%.5f", num);
  Str retorno = s_cria(temp);
  return retorno;
}

Str s_cria_unindo(Lista l, Str sep){
  s_ok(sep);
  Str retorno = s_cria("");
  int total = l_tam(l);
  for (int i = 0; i < total; i++){
    dado_t elemento = l_dado_pos(l, i);
    if (elemento != NULL){
      s_anexa(retorno, elemento);
    }
    if (sep != NULL && i < total - 1){
      s_anexa(retorno, sep);
    }
    
  }
  return retorno;
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
  FILE *arquivo;
  arquivo = fopen(nome, "r");
  if(arquivo == NULL){
    return s;
  }
  fseek(arquivo, 0, SEEK_END);
  long cursor = ftell(arquivo);
  s->string = realloc(s->string, (cursor + 1) * sizeof(char));
  fseek(arquivo, 0, SEEK_SET);
  fread(s->string, sizeof(char), cursor, arquivo);
  s->string[cursor] = '\0';
  byte *copia = s->string;
  int x = strlen(s->string);
  s->bytesAlocados = x;
  s->tamanhoCaracteres = u8_conta_unichar_nos_bytes(x, copia);
  return s;
}

// operações de acesso {{{1

int s_tam(Str_c s)
{
  s_ok(s);
  int x;
  if (s->string == NULL){
    x = 0;
  } else{
    x = strlen(s->string);
  }
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
  int x = strlen(s->string);
  char *copia;
  copia = malloc(alocacao(x) + 1);
  strcpy(copia, s->string);
  return copia;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  arrumaPos(&pos, s->string);
  if (pos > s_tam(s) || s->string == NULL){
    return UNI_INV;
  }
  byte *ptrChar = u8_avanca_unichar(s->string, pos);
  int bytesRestantes = strlen(ptrChar);
  unichar codificacao;
  int x = u8_unichar_nos_bytes(bytesRestantes ,ptrChar, &codificacao); 
  if (x == -1)
  {
    return x;
  }
  return codificacao;
}

double s_número(Str_c s)
{
  s_ok(s);
  if (s->string == NULL) return 0.0;
  double resultado;
  sscanf(s->string, "%lf", &resultado);
  return resultado;
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
  arrumaPos(&pos, s->string);
  int incrementaPos = pos;
  while(incrementaPos < s_tam(s)){
    byte *ptr = u8_avanca_unichar(s->string, incrementaPos);
    unichar c;
    u8_unichar_nos_bytes(strlen(ptr), ptr,&c);
    if (uniIgual(c, sb) == true){
      return incrementaPos;
    } else{
      incrementaPos+= 1;
    }
  }
  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  arrumaPos(&pos, s->string);
  int incrementaPos = pos;
  while(incrementaPos < s_tam(s)){
    byte *ptr = u8_avanca_unichar(s->string, incrementaPos);
    unichar c;
    u8_unichar_nos_bytes(strlen(ptr), ptr,&c);
    if (uniIgual(c, sb) == false){
      return incrementaPos;
    } else{
      incrementaPos+= 1;
    }
  }
  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  arrumaPos(&pos, s->string);
  int incrementaPos = pos;
  while(incrementaPos >= 0){
    byte *ptr = u8_avanca_unichar(s->string, incrementaPos);
    unichar c;
    u8_unichar_nos_bytes(strlen(ptr), ptr,&c);
    if (uniIgual(c, sb) == true){
      return incrementaPos;
    } else{
    incrementaPos-= 1;
    }
  }
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  arrumaPos(&pos, s->string);
  int incrementaPos = pos;
  while(incrementaPos >= 0){
    byte *ptr = u8_avanca_unichar(s->string, incrementaPos);
    unichar c;
    u8_unichar_nos_bytes(strlen(ptr), ptr,&c);
    if (uniIgual(c, sb) == false){
      return incrementaPos;
    } else{
    incrementaPos-= 1;
    }
  }
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  arrumaPos(&pos, s->string);
  for (int i = pos; i <= s_tam(s) - s_tam(buscada); i++){
    bool achouTodas = true;
    for (int j = 0; j < s_tam(buscada); j++){
      unichar c = s_ch(s, i);
      unichar cSb = s_ch(buscada, j);
      if (c != cSb){
        achouTodas = false;
        break;
      }      
    }
    if (achouTodas == true){
      return i;
    }  
  }  
  return -1;
}


// operações de alteração {{{1

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  if(sb != NULL){
    s_ok(sb);
  }
  s_ok(s);
  arrumaPos(&pos, s->string);
  arrumaTam(&tam, &pos, s->string);
  byte *ptrIni = u8_avanca_unichar(s->string, pos);
  byte *ptrFim = u8_avanca_unichar(ptrIni, tam);
  if (sb == NULL){
    byte *ptrFimString = s->string + strlen(s->string); 
    int bytesRemover = ptrFim - ptrIni;
    int bytesRestantes = ptrFimString - ptrFim;
    for (int k = 0; k <= bytesRestantes; k++){
    ptrIni[k] = ptrFim[k];
    }
  } else{
    byte *ptrFimString = s->string + strlen(s->string);
    int bytesSb = strlen(sb->string);
    int bytesRemover = ptrFim - ptrIni;
    int bytesRestantes = ptrFimString - ptrFim;
    int bytesTotais = bytesSb + bytesRestantes;
    int x = alocacao(bytesTotais);
    char *texCorreto = malloc(x);
    int j = 0;
    for (int i = 0; i < bytesSb; i++) {
      texCorreto[j++] = sb->string[i];
    }
    for (int i = 0; i <= bytesRestantes; i++) {
      texCorreto[j++] = ptrFim[i];
    }
    texCorreto[j] = '\0';
    free(s->string);
    s->string = texCorreto;
  }
  
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  s_ok(s);
  s_ok(sb);
  arrumaPos(&pos, sb->string);
  arrumaTam(&tam, &pos, sb->string);
  byte *inicio = u8_avanca_unichar(sb->string, pos);
  byte *fim = u8_avanca_unichar(inicio, tam);
  int bytesTotais = fim - inicio;
  if(s->string != NULL){
  free(s->string);
  }
  int bytesAlocados = alocacao(bytesTotais);
  s->string = malloc(bytesAlocados);
  int k = 0;
  while (inicio < fim)
  {
    int codigo = u8_nbytes_no_unichar_que_comeca_com(*inicio);
    for(int i = 0; i < codigo; i++){
      s->string[k] = inicio[i];
      k++;
    }
    inicio = inicio + codigo;
  }
  s->string[k] = '\0';
  s->bytesAlocados = bytesAlocados;
  s->tamanhoCaracteres = tam;
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
  int *Ppos = &pos;
  byte *copia = s->string;
  arrumaPos(Ppos, copia);
  int tamC = u8_converte_pra_utf8(c, NULL);
  int tamS;
  if (s->string != NULL){
    tamS = strlen(s->string);
  } else {
    tamS = 0;
  }
  byte *posicao = u8_avanca_unichar(s->string, pos);
  int tamTotal = tamC + tamS;
  int posByte = posicao - (byte *)s->string;
  s->string = realloc(s->string, alocacao(tamTotal));
  posicao = (byte *)s->string + posByte;
  for (int i = tamS; i >= posByte; i--)
  {
    s->string[i + tamC] = s->string[i];
  }
  u8_converte_pra_utf8(c, posicao);
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
  while (s_tam(s) > 0){
    bool removeu = false;
    unichar c = s_ch(s, 0);
    if(uniIgual(c, sobras) == true){
      s_remove(s, 0, 1);
      removeu = true;
    }
    int posFim = s_tam(s) - 1;
    unichar cF = s_ch(s, posFim);
    if (uniIgual(cF, sobras) == true)
    {
      s_remove(s, posFim, 1);
      removeu = true;
    }
    if (removeu == false){
      break;
    }
  }
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);
  if (s->string == NULL)
  {
    printf("");
  }else{
    printf("%s", s->string);
  }
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
  FILE *arquivo;
  arquivo = fopen(nome, "w");
  assert(arquivo != NULL);
  fprintf(arquivo, "%s", s->string);
  fclose(arquivo);
}


// vim: foldmethod=marker shiftwidth=2

