#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <sys/types.h> 
#include <sys/wait.h> 
/* La funzione viene utilizzata per attendere i cambiamenti di stato del programma
nei processi figli e recuperare le informazioni corrispondenti.*/
#include <time.h>   
/*time. h è l'header file della libreria standard del C che fornisce un accesso standardizzato 
alle funzioni di acquisizione e manipolazione del tempo.*/
#include <string.h> 
/*string.h è l'header file della libreria standard del C che contiene definizioni di macro, 
costanti e dichiarazioni di funzioni e tipi usati non solo nella manipolazione delle stringhe 
ma anche nella manipolazione della memoria.*/

#define N_ALIENI 	1
#define	UP			65
#define DW			66
#define	SX 			68
#define DX			67
#define SPACE   32
#define	MAXX		130
#define MAXY		36
#define DELAY 	80000
#define VITE    3
#define VITE_ALIENO     3
#define VITE_ALIENOLV2  2

/*infopipe indichera una struttura che contiene le coordinate x,y , l'id , il pid, e lo status dell'elemento che andremo a prendere in considerazione*/
typedef struct infoPipe{ 
  int x;          //coordinata x
  int y;          //coordinata y
  int id;         //id 
  int pid;        //identificatore univoco del processo nel sistema
  int morto;      //status 
}InfoPipe;

void Navicella(int pipe);   /*processo della navicella*/
void Alieno(int pipe, int id); /*processo relativo alle navicelle nemiche*/
void Controllo(int pipe, int pidAlieno[]); /*procedura di controllo delle collisioni e della stampa*/
void missileUp (int pipe, InfoPipe infoNavicella, int id); /*procedura che controlla il missile che sale verso l'alto*/
void missileDw (int pipe, InfoPipe infoNavicella, int id); /*procedura che controlla il missile che sale verso il basso*/
void bomba(int pipe, InfoPipe info, int id); /*funzione che controlla le bombe lanciate dai nemici e le rispettive coordinate*/
void stampaNavicella (InfoPipe info); /* funzione che stampa la navicella (full vita) come matrice 3x3*/
void stampaNavicella1 (InfoPipe info);/* funzione che stampa la navicella1 (meta vita) come matrice 3x3*/
void stampaNavicella2 (InfoPipe info);/* funzione che stampa la navicella2 (una vita) come matrice 3x3*/
void cancella3x3 (InfoPipe info); /*funzione che elimina lo sprite 3x3*/
void stampaAlienolv1 (InfoPipe info); /*funzione che stampa l'alieno (full vita)*/
void stampaAlienolv2 (InfoPipe info); /*funzione che stampa l'alieno (meta vita)*/
void stampaAlienolv2Half (InfoPipe info); /*funzione che stampa l'alieno (ultima vita)*/
/*void Background(int pipe); */
char* generaBackground(); /*funzione che genera un background*/
char* spostaBackground(char* background); /*funzione che fa muovere il background*/
void stampaBackground (char* background); /*funzione che stampa? un background*/
char* generaBackgroundVuoto(); /*funzione che genera un background vuoto, da riempire successivamente*/
int endgameCheck (InfoPipe* infoAli, InfoPipe infoNav);/*funzione che determina le condizioni di fine gioco*/
