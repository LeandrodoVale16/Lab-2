#define _POSIX_C_SOURCE 199309L //jeito que encontrei para que o CLOCK_MONOTIC rodasse
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct timespec crono;

typedef struct{
    bool diurno, noturno;
}modo;

typedef struct{
  int escudos, municao, pontos, coluna, quantidadeInimigos, codigoSom;
  int melhoresPontos[3];
  float velocidade;
  char arma;
  char posicoes[14];
  modo *modoAtual;
  bool vivo, mostrou, continuar, decidiu;
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

//inicializa todos os estados (ou quase)
void inicializaEstado(estado *e){
  e->pontos = 0;
  e->municao = 30;
  e->escudos = 3;
  e->quantidadeInimigos = 0;
  e->arma = '0';
  e->coluna = 10;
  e->velocidade = 2.0;
  e->vivo = true;
  e->continuar = true;
  e->decidiu = false;
  e->mostrou = false;
  strcpy(e->posicoes, ")))          ");
}

void verificaPontos(estado *e){
    FILE *arquivo;
    arquivo = fopen("pontuacao.txt", "r");
    if(arquivo == NULL){
      e->melhoresPontos[0] = 0;
      e->melhoresPontos[1] = 0;
      e->melhoresPontos[2] = 0;
      return;
    }
    
    fscanf(arquivo, "%d %d %d", &e->melhoresPontos[0], &e->melhoresPontos[1], &e->melhoresPontos[2]);
    fclose(arquivo);
}

int menorPontuacao(estado *e){
    int menor = 0;
    for (int i = 1; i < 3; i++){
        if (e->melhoresPontos[i] < e->melhoresPontos[menor]){
            menor = i;
        }
    }
    return menor;
}

void escrevePontos(estado *e){
    FILE *arquivo;
    int menor = menorPontuacao(e);
    if(e->pontos > e->melhoresPontos[menor]){
    arquivo = fopen("pontuacao.txt", "w");
      if(arquivo == NULL){
          printf("não foi possivel salvar sua pontuação\n");
          return;
      }

      fprintf(arquivo, "%d %d %d", e->melhoresPontos[0], e->melhoresPontos[1], e->melhoresPontos[2]);
      fclose(arquivo);
      printf(" novo recorde!");
    }
}

void sons(estado *e){
  char som[100];
  sprintf(som, "aplay -q Sons/%d.2.wav &", e->codigoSom);
  system(som);
}

void sonsSonar(estado *e){
  char som[100];
  sprintf(som, "aplay -q Sons/%d.4.wav", e->codigoSom);
  system(som);
}

void sonar(estado *e, char tecla){
    if (tecla == 32) {
        for (int i = 0; i < 13; i++){
            if (e->posicoes[i] == ')'){
                e->codigoSom = 12;
                sonsSonar(e);
            } else if (e->posicoes[i] == ' '){
                system("aplay -q Sons/x.4.wav");
            } else if (e->posicoes[i] == 'n'){
                e->codigoSom = 10;
                sonsSonar(e);
            } else if (e->posicoes[i] == 'N'){
                e->codigoSom = 11;
                sonsSonar(e);
            } else{
                e->codigoSom = e->posicoes[i] - '0';
                sonsSonar(e);
            }
        } 
    }
}

char resumo(estado *e){
  char tecla = lechar();
  while(tecla != 'r'){
    putchar('\r');
    printf("pontos: %d escudos: %d.'r' para continuar", e->pontos, e->escudos);
    escrevePontos(e);
    tecla = lechar();
  }
  return tecla;
}

//aparece todas as informaçoes do jogo
void tela(estado *e){
  printf("%d %d %c", e->pontos, e->municao, e->arma);
  printf("%s", e->posicoes);
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

//coloca o inimigo na ultima posição
void colocaInimigos(estado *e){
  if (e->quantidadeInimigos != 20){
    e->posicoes[12] = geraInimigos(e);
    sons(e);
  }
}

//troca a arma com tab
void trocaArma(estado *e, char tecla){
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

//da o tiro, soma os pontos e diminui a munição ao apertar enter
void tiro(estado *e, char tecla){
    int x = 1;
    if (tecla == '\r' && e->municao > 0){
        for (int i = 0; i < 13; i++){
            if (e->posicoes[i] == e->arma && e->arma != 'n'){
                e->posicoes[i] = ' ';
                e->pontos = e->pontos + x;
                break;
            } else if (e->posicoes[i] == e->arma && e->arma == 'n'){
                e->posicoes[i] = ' ';
                e->pontos = e->pontos + (x * 2);
                break;
            } else if (e->arma == 'n' && e->posicoes[i] == 'N'){
                e->posicoes[i] = 'n';
                e->codigoSom = 11;
                sons(e);
                break;
            }
            x++;
        } 
        e->municao--;
    }
}

//reconhece se o jogador perdeu
void perdeu(estado *e){
  if ((e->posicoes[0] >= '0' && e->posicoes[0] <= '9') || e->posicoes[0] == 'N' ||  e->posicoes[0] == 'n'){
    e->vivo = false;
  }
}

// coloca e movimenta os inimigos, além de mostrar a mensagem de jogo perdido
void movimentaInimigos(estado *e, crono *c){
  if (crono_parcial(c)  >= e->velocidade){
    perdeu(e);
    for (int i = 1; i < 14; i++){
      if ((e->posicoes[i] >= '0' && e->posicoes[i] <= '9') || (e->posicoes[i] == 'N' ||  e->posicoes[i] == 'n')){
        if (e->posicoes[i - 1] == ')'){
          e->posicoes[i - 1] = ' ';
          e->posicoes[i] = ' ';
          e->escudos--;
        } else {
          e->posicoes[i - 1] = e->posicoes[i];
          e->posicoes[i] = ' ';
        }
        }
    }
    if(e->quantidadeInimigos < 20) {
      colocaInimigos(e);
      e->quantidadeInimigos++;
    }
    crono_inicia(c);
   } 
}

//identifica se tem inimigo
bool identificaInimigo(estado *e){
 bool temInimigo = false;
 for (int i = 0; i < 14; i++){
 if ((e->posicoes[i] >= '0' && e->posicoes[i] <= '9') || (e->posicoes[i] == 'N' ||  e->posicoes[i] == 'n')){
  temInimigo = true;
  }
 }
 return temInimigo;
}

//verifica se acabou a rodada e se o jogador quer continuar, se sim, ajeita uma proxima rodada
bool rodada(estado *e) {
  bool acabou = false;
  if (e->quantidadeInimigos == 20) {
    if (identificaInimigo(e) == false && e->vivo == true){
      e->pontos = e->pontos + (10 * e->escudos);
      putchar('\n');
      if (resumo(e) == 'r'){
        putchar('\n');
        e->velocidade = e->velocidade - (e->velocidade / 10);
        e->municao = 30;
        e->quantidadeInimigos = 0;
        e->mostrou = false;
        return acabou = true;
      }
    }
  }
  return acabou;
}

//faz todo modo diurno
void diurno(estado *e){
  char tecla;
  crono tempoInimigos;
  crono_inicia(&tempoInimigos);
  while (e->vivo == true ){
    if (tecla == 27){
      e->vivo = false;
      e->continuar = false;
      break;
    }
    trocaArma(e, tecla);
    tiro(e, tecla);
    movimentaInimigos(e, &tempoInimigos);
    sonar(e, tecla);
    tela(e);
    rodada(e);
    putchar('\r');
    tecla = lechar();
  }
}

//habilita o jogador dizer se quer ou não continuar
void continuar(estado *e){
  escrevePontos(e);
  while (e->decidiu == false){
    telaEscolha(e);
    char tecla = lechar();
    if (tecla == 'S' || tecla == 's'){
      e->decidiu = true;
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
  diurno(e);
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