#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
/*Librerie per la manipolazione dei processi */
#include <sys/types.h>
#include <sys/wait.h>

#define LIVELLO
#define VITE

#define UP        65 /* Cursore sopra */
#define DW        66 /* Cursore sotto */
#define SX        68 /* Cursore sinistra */
#define DX        67 /* Cursore destra */
#define MAXX      80 /* Dimensione dello schermo di output(colonne) */
#define MAXY      24 /* Dimensione dello schermo di output (righe) */
#define DELAY  20000 /* Ritardo del movimento delle navicelle nemiche (da adattare) */

/* Struttura adoperata per veicolare le coordinate */
typedef struct{
	int  x; /* Coordinata x */
	int  y; /* Coordinata y */
	char c; /* Identificatore personaggio */
}pos;

main(){

  int p[2];           /* Filedescriptor -> nome della pipe */
  int pidAstronave;   /* Pid processo figlio Astronave */
	int pidNavicelle;		//Pid processo figlio Navicelle Nemiche

  initscr();         /* Inizializza schermo di output */
  noecho();          /* Imposta modalita' da tastiera */
  curs_set(0);       /* Nasconde il cursore */
	getmaxyx(stdscr, MAXY, MAXX);

  srand(time(NULL)); /* Inizializza seme generatore di passi casuali */

  /* Creazione della pipe e controllo errori invocazione */
if(pipe(p)==-1){
	perror("Errore nella creazione della pipe!");
	_exit(1);
}

pidAstronave=fork(); /* Creazione del primo processo figlio Astronave Madre */

switch(pidAstronave){
  case -1:
    perror("Errore nell'esecuzione della fork.");
    _exit(2);
  case 0:
    close(p[0]);
    Astronave(p[1]);
  default:
	pidNavicelle=fork();
}

switch(pidNavicelle){
	case -1:
    perror("Errore nell'esecuzione della fork.");
    _exit(2);
  case 0:
    close(p[0]);
    Navicelle(p[1]);
  default:
}
  return 0;
}
