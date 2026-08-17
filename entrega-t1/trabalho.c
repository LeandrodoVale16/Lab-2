#define _POSIX_C_SOURCE 199309L //jeito que encontrei para que o CLOCK_MONOTIC rodasse
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct timespec crono;

typedef struct{
  int escudos, municao, pontos, coluna, quantidadeInimigos, codigoSom;
  int porcentagemModo;
  int topRank[3];
  float velocidade, velocidadeNoturna;
  char arma;
  char P[14];
  char PN[9];
  bool vivo, mostrou, continuar, decidiu, salvo, diurno;
}estado;

void crono_inicia(crono *c){
  clock_gettime(CLOCK_MONOTONIC, c);
}

double crono_parcial(crono *c){
  crono agora;
  clock_gettime(CLOCK_MONOTONIC, &agora);

  double segundos = agora.tv_sec - c->tv_sec;
  double nanosegundos = agora.tv_nsec - c->tv_nsec;
  return segundos + 1e-9 * nanosegundos;
}

char lechar()
{
  fflush(stdout);
  char c;
  if (fread(&c, 1, 1, stdin) == 1) return c;
  return 0;
}

//verifica e guarda os pontos
void verificaPontos(estado *e){
  FILE *arquivo;
  arquivo = fopen("pontuacao.txt", "r");
  if (arquivo == NULL){
    e->topRank[0] = 0;
    e->topRank[1] = 0;
    e->topRank[2] = 0;
    return;
  }  
  fscanf(arquivo, "%d %d %d", &e->topRank[0], &e->topRank[1], &e->topRank[2]);
  fclose(arquivo);
}

//retorna o indice do menor ponto
int menorPontuacao(estado *e){
  int menor = 0;
  for (int i = 1; i < 3; i++){
    if (e->topRank[i] < e->topRank[menor]){
      menor = i;
    }
  }
  return menor;
}

//sobrescreve o menor ponto
void escrevePontos(estado *e){
  FILE *arquivo;
  int menor = menorPontuacao(e);
  if (e->salvo == true){
    return;
  }
  if (e->pontos > e->topRank[menor]){
    arquivo = fopen("pontuacao.txt", "w");
    if (arquivo == NULL){
      printf("não foi possivel salvar sua pontuação\n");
      return;
    }
    e->topRank[menor] = e->pontos;
      fprintf(arquivo, "%d %d %d", e->topRank[0], e->topRank[1], e->topRank[2]);
      e->salvo = true;
    fclose(arquivo);
    printf("novo recorde!\n");
  }
}

//inicializa todos os estados (ou quase)
void inicializaEstado(estado *e){
  e->pontos = 0;
  e->municao = 30;
  e->escudos = 3;
  e->quantidadeInimigos = 0;
  e->arma = '0';
  e->coluna = 10;
  e->velocidade = 2.0;
  e->porcentagemModo = 100;
  e->diurno = true;
  e->vivo = true;
  e->continuar = true;
  e->salvo = false;
  e->decidiu = false;
  e->mostrou = false;
  e->velocidadeNoturna = e->velocidade * 3;
  strcpy(e->P, ")))          ");
  strcpy(e->PN, "        ");
}

//toca sons
void sons(estado *e){
  char som[100];
  sprintf(som, "aplay -q Sons/%d.2.wav &", e->codigoSom);
  system(som);
}

//toca sons para o sonar
void sonsSonar(estado *e){
  char som[100];
  sprintf(som, "aplay -q Sons/%d.4.wav", e->codigoSom);
  system(som);
}

//modo do sonar
void sonar(estado *e, char tecla){
  if (tecla == ' ') {
    for (int i = 0; i < 8; i++){
      if (e->PN[i] == ')'){
        e->codigoSom = 12;
        sonsSonar(e);
      } else if (e->PN[i] == ' '){
        system("aplay -q Sons/x.4.wav");
      } else if ( e->PN[i] == 'n'){
        e->codigoSom = 10;
        sonsSonar(e);
      } else if (e->PN[i] == 'N'){
        e->codigoSom = 11;
        sonsSonar(e);
      } else {
        e->codigoSom = e->PN[i] - '0';
        sonsSonar(e);
      }
    } 
  }
}

//sorteia o modo
void modo(estado *e){
  int x = rand() % 100;
  if (x < e->porcentagemModo){
    e->diurno = true;
  } else {
    e->diurno = false;
  }
}


//resumo da partida
char resumo(estado *e){
  char tecla = lechar();
  if (e->vivo == true){
    e->salvo = false;
    verificaPontos(e);
    while(tecla != 'r'){
      putchar('\r');
      printf("pontos: %d escudos: %d.'r' para continuar", e->pontos, e->escudos);
      tecla = lechar();
    }
  }
  return tecla;
}

//aparece todas as informaçoes do jogo
void tela(estado *e){
  if (e->diurno == true){
    printf("%d %d %c", e->pontos, e->municao, e->arma);
    printf("%s", e->P);
  } else {
    printf("MODO NOTURNO");
  }
}

//pergunta se o jogador quer continuar
void telaEscolha(estado *e){
  if (e->decidiu == false){
    printf("você perdeu, continuar? (s/n)");
  }
}

//gera os inimigos
char geraInimigos(estado *e){
  int x = rand() % 11;
  if (x == 10){
    char h = x + 20 + '0';
    e->codigoSom = 11;
    return h;
  } else {
    char h = x + '0';
    e->codigoSom = x;
    return h;
  }
}

//gera os inimigos no modo noturno
char geraInimigosNoturnos(estado *e){
  int x = rand() % 11;
  if (x % 2 == 1 && x != 9){
    x = x + 1;
    char h = x + '0';
    e->codigoSom = x;
    return h;
  } else if (x == 9){
    x = x - 1;
    char h = x + '0';
    e->codigoSom = x;
    return h;
  }else if (x == 10){
    char h = x + 20 + '0';
    e->codigoSom = 11;
    return h;
  } else {
    char h = x + '0';
    e->codigoSom = x;
    return h;
  }
}

//coloca o inimigo na ultima posição
void colocaInimigos(estado *e){
  if (e->diurno == true){
    if (e->quantidadeInimigos != 20){
      e->P[12] = geraInimigos(e);
      sons(e);
    }
  } else {
    if (e->quantidadeInimigos != 15){
      e->PN[7] = geraInimigosNoturnos(e);
      sons(e);
    }
  }
}

//troca a arma com tab e toca som
void trocaArmaDiurno(estado *e, char tecla){
  if (e->diurno == true){
    if (tecla == '\t'){
      if (e->arma == '9'){
        e->arma = 'n';
        e->codigoSom = 10;
        sons(e);
      } else if (e->arma == 'n'){
        e->arma = '0';
        e->codigoSom = e->arma - '0';
        sons(e);
      } else {
        e->arma = e->arma + 1;
        e->codigoSom = e->arma - '0';
        sons(e);
      }
    }
  }
}

//mesma coisa do trocaArmaDiurno só que pro modo noturno
void trocaArmaNoturno(estado *e, char tecla){
  if (e->diurno == false){
    if (tecla == '\t'){
      if (e->arma == '8'){
        e->arma = 'n';
        e->codigoSom = 10;
        sons(e);
      } else if (e->arma == 'n'){
        e->arma = '0';
        e->codigoSom = e->arma - '0';
        sons(e);
      } else {
        e->arma = e->arma + 2;
        e->codigoSom = e->arma - '0';
        sons(e);
      }
    }
  }
}

//consome os inimigos, adiciona os pontos e toca o som se acertou ou errou
void consomeInimigoDiurno(estado *e){
  int x = 13;
  bool matou = false;
  for (int i = 0; i < 13; i++){
    if (e->P[i] == e->arma && e->arma != 'n'){
      e->P[i] = ' ';
      matou = true;
      e->pontos = e->pontos + x;
      e->codigoSom = e->arma - '0';
      sons(e);
      break;
    } else if (e->P[i] == e->arma && e->arma == 'n'){
      e->P[i] = ' ';
      matou = true;
      e->pontos = e->pontos + (x * 2);
      e->codigoSom = 10;
      sons(e);
      break;
    } else if (e->arma == 'n' && e->P[i] == 'N'){
      e->P[i] = 'n';
      matou = true;
      e->codigoSom = 11;
      sons(e);
      break;
    } else if (i == 12 && matou == false){
      system("aplay -q Sons/x.2.wav");
    }
    x--;
  }
}

//consome os inimigos so que no modo noturno
void consomeInimigoNoturno(estado *e){
  int x = 8;
  bool matou = false;
  for (int i = 0; i < 8; i++){
    if (e->PN[i] == e->arma && e->arma != 'n'){
      e->PN[i] = ' ';
      matou = true;
      e->pontos = e->pontos + x;
      e->codigoSom = e->arma - '0';
      sons(e);
      break;
    } else if (e->PN[i] == e->arma && e->arma == 'n'){
      e->PN[i] = ' ';
      matou = true;
      e->pontos = e->pontos + (x * 2);
      e->codigoSom = 10;
      sons(e);
      break;
    } else if (e->arma == 'n' && e->PN[i] == 'N'){
      e->PN[i] = 'n';
      matou = true;
      e->codigoSom = 11;
      sons(e);
      break;
    } else if (i == 7 && matou == false){
      system("aplay -q Sons/x.2.wav");
    }
    x--;
  }
}

//da o tiro, chama consomeInimigo e diminui a munição
void tiro(estado *e, char tecla){
  if (tecla == '\r' && e->municao > 0){
    if (e->diurno == true){
      consomeInimigoDiurno(e);
    } else {
      consomeInimigoNoturno(e);
    }
    e->municao--;
  }
}

//reconhece se o jogador perdeu
void perdeu(estado *e){
  if (e->diurno == true){
    if ((e->P[0] >= '0' && e->P[0] <= '9') || e->P[0] == 'N' || e->P[0] == 'n'){
      e->vivo = false;
    }
  } else{
    if ((e->PN[0] >= '0' && e->PN[0] <= '9') || e->PN[0] == 'N' || e->PN[0] == 'n'){
      e->vivo = false;
    }
  }
}

// coloca e movimenta os inimigos, além de mostrar a mensagem de jogo perdido
void movimentaInimigos(estado *e, crono *c){
  if (crono_parcial(c)  >= e->velocidade){
    perdeu(e);
    for (int i = 1; i < 14; i++){
      if ((e->P[i] >= '0' && e->P[i] <= '9') || (e->P[i] == 'N' || e->P[i] == 'n')){
        if (e->P[i - 1] == ')'){
          e->P[i - 1] = ' ';
          e->P[i] = ' ';
          e->escudos--;
        } else {
          e->P[i - 1] = e->P[i];
          e->P[i] = ' ';
        }
      }
    }
    if (e->quantidadeInimigos < 20) {
      colocaInimigos(e);
      e->quantidadeInimigos++;
    }
    crono_inicia(c);
  } 
}

// movimentaInimigos só que pro modo noturno
void movimentaInimigosNoturnos(estado *e, crono *c){
  if (crono_parcial(c)  >= e->velocidadeNoturna){
    perdeu(e);
    for (int i = 1; i < 8; i++){
      if ((e->PN[i] >= '0' && e->PN[i] <= '8') || (e->PN[i] == 'N' || e->PN[i] == 'n')){
        if (e->PN[i - 1] == ')'){
          e->PN[i - 1] = ' ';
          e->PN[i] = ' ';
          e->escudos--;
        } else {
          e->PN[i - 1] = e->PN[i];
          e->PN[i] = ' ';
        }
      }
    }
    if (e->quantidadeInimigos < 15) {
      colocaInimigos(e);
      e->quantidadeInimigos++;
    }
    crono_inicia(c);
  } 
}

//identifica se tem inimigo
bool identificaInimigo(estado *e){
  bool temInimigo = false;
  if (e->diurno == true){
    for (int i = 0; i < 14; i++){
      if ((e->P[i] >= '0' && e->P[i] <= '9') || (e->P[i] == 'N' ||  e->P[i] == 'n')){
        temInimigo = true;
      }
    }
  } else {
    for (int i = 0; i < 9; i++){
      if ((e->PN[i] >= '0' && e->PN[i] <= '9') || (e->PN[i] == 'N' ||  e->PN[i] == 'n')){
        temInimigo = true;
      }
    }
  }
  return temInimigo;
}

//verifica se acabou a rodada e se o jogador quer continuar, se sim, ajeita uma proxima rodada
bool rodada(estado *e) {
  bool acabou = false;
  if ((e->diurno == true && e->quantidadeInimigos == 20) || (e->diurno == false && e->quantidadeInimigos == 15)) {
    if (identificaInimigo(e) == false && e->vivo == true){
      verificaPontos(e);
      e->pontos = e->pontos + (10 * e->escudos);
      putchar('\n');
      if (resumo(e) == 'r'){
        putchar('\n');
        modo(e);
        e->velocidade = e->velocidade - (e->velocidade / 10);
        if (e->diurno == false){
          e->velocidadeNoturna = e->velocidade * 3;
        }
        e->municao = 30;
        e->quantidadeInimigos = 0;
        if (e->porcentagemModo > 20){
          e->porcentagemModo = e->porcentagemModo - 20;
        }
        e->mostrou = false;
        return acabou = true;
      }
    }
  }
  return acabou;
}

void escudosNoturno(estado *e){
  for (int i = 0; i < e->escudos; i++){
    e->PN[i] = ')';
  }
}

//faz todo o modo noturno
void noturno(estado *e){
  char tecla = '0';
  e->arma = '0';
  escudosNoturno(e);
  crono tempoInimigos;
  crono_inicia(&tempoInimigos);
  while (e->vivo == true && e->diurno == false){
    if (tecla == 27){
      e->vivo = false;
      e->continuar = false;
      verificaPontos(e);
      escrevePontos(e);
      break;
    }
    trocaArmaNoturno(e, tecla);
    tiro(e, tecla);
    movimentaInimigosNoturnos(e, &tempoInimigos);
    sonar(e, tecla);
    tela(e);
    rodada(e);
    putchar('\r');
    tecla = lechar();
  }
}

//faz todo modo diurno
void diurno(estado *e){
  char tecla = '0';
  crono tempoInimigos;
  crono_inicia(&tempoInimigos);
  while (e->vivo == true && e->diurno == true){
    if (tecla == 27){
      e->vivo = false;
      e->continuar = false;
      verificaPontos(e);
      escrevePontos(e);
      break;
    }
    trocaArmaDiurno(e, tecla);
    tiro(e, tecla);
    movimentaInimigos(e, &tempoInimigos);;
    tela(e);
    rodada(e);
    putchar('\r');
    tecla = lechar();
  }
}

//diz pro programa se ta no modo diurno ou não
void selecionaModo(estado *e){
  while (e->vivo == true){
    if (e->diurno == false){
      noturno(e);
    } else {
      diurno(e);
    }
  }
}

//habilita o jogador dizer se quer ou não continuar
void continuar(estado *e){
  e->salvo = false;
  verificaPontos(e);
  escrevePontos(e);
  putchar('\n');
  while (e->decidiu == false){
    telaEscolha(e);
    char tecla = lechar();
    if (tecla == 'S' || tecla == 's'){
      e->decidiu = true;
      e->salvo = false;
      printf("\n");
    } else if (tecla == 'N' || tecla == 'n'){
      e->continuar = false;
      e->decidiu = true;
    } 
    putchar('\r');    
  }
}

//todo o jogo roda aqui
void jogo(estado *e) {
  inicializaEstado(e);
  verificaPontos(e);
  selecionaModo(e);
  if (e->continuar == true){
    continuar(e);
  }
}    

//inicia coisas necessarias para o funcionamento do jogo
void arrumaGame(){
  srand(time(NULL));
  system("stty raw -echo min 0 time 1 opost");
  setvbuf(stdin, NULL, _IONBF, 0);
}

int main(){
  arrumaGame();
  estado e;
  inicializaEstado(&e);
  while (e.continuar == true){
    jogo(&e);
  }
  system("stty sane");
  return 0;
}