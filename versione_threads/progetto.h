#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define N_ALIENI 	9
#define	UP			65
#define DW			66
#define	SX 			68
#define DX			67
#define SPACE   32
#define	MAXX		131
#define MAXY		30
#define DELAY 	80000
#define VITE    3
#define VITE_ALIENO     3
#define VITE_ALIENOLV2  2
#define DIM_BUFFER 50

/*
Creo una struttura per imitare il comportamento delle pipe come nei processi, solo che qui stiamo utilizzando i thread
*/
typedef struct infoPipe{
  int x;//coordinata x
  int y;//coordinata y
  int morto;//status 
  char type;//serve a differenziare gli oggetti tra di loro
  int id;//id dell'elemento 
  pthread_t tid;//id del thread appartenente ad un elemento
}InfoPipe;



//VARIABILI GLOBALI
pthread_t tMissileUp, tMissileDw, tNavicella, tAlieno, tControllo, tBackground, tBomba;
InfoPipe* buffer;
pthread_mutex_t	mtx; //semaforo utilizzato per gestire la sezione critica
sem_t vuoto; //per sapere se l'array buffer sia pieno o vuoro
sem_t pieno;
int posIn, posOut; //servono per scorrere l'array e salvare l'informazione nella cella successiva/precedente del buffer
int contatoreIdAlieni;
int contatoreIdMissili;
int contatoreIdBombe;

void* Navicella(void* arg);/*processo della navicella*/
void* Alieno(void* arg);/*processo relativo alle navicelle nemiche*/
void* Background(void* arg);//processo che permette di far entrare il background dentro il buffer e aggiornarlo di volta in volta
void Controllo(); /*procedura di controllo delle collisioni e della stampa*/
void* missileUp (void* arg);/*procedura che controlla il missile che sale verso l'alto*/
void* missileDw (void* arg); /*procedura che controlla il missile che sale verso il basso*/
void* bomba(void* arg);/*funzione che controlla le bombe lanciate dai nemici e le rispettive coordinate*/
void stampaNavicella (InfoPipe info);/* funzione che stampa la navicella (full vita) come matrice 3x3*/
void stampaNavicella1 (InfoPipe info);/* funzione che stampa la navicella1 (meta vita) come matrice 3x3*/
void stampaNavicella2 (InfoPipe info);/* funzione che stampa la navicella2 (una vita) come matrice 3x3*/
void cancella3x3 (InfoPipe info);/*funzione che elimina lo sprite 3x3*/
void stampaAlienolv1 (InfoPipe info);/*funzione che stampa l'alieno (full vita)*/
void stampaAlienolv2 (InfoPipe info); /*funzione che stampa l'alieno (meta vita)*/
void stampaAlienolv2Half (InfoPipe info);/*funzione che stampa l'alieno (ultima vita)*/
char* generaBackground();/*funzione che genera un background*/
char* spostaBackground(char* background);/*funzione che fa muovere il background*/
void stampaBackground (char* background);/*funzione che stampa? un background*/
char* generaBackgroundVuoto();/*funzione che genera un background vuoto, da riempire successivamente*/
int endgameCheck (InfoPipe* infoAli, InfoPipe infoNav);/*funzione che determina le condizioni di fine gioco*/
void bufferIn(InfoPipe info); //funzione che crea l'ingresso di un buffer condiviso dai thread
InfoPipe bufferOut(); //funzione che crea l'uscita di un buffer condiviso dai thread
void setArgs(InfoPipe info, int id, int* args); 
