/* Invasione Aliena - 3 Dicembre 2020*/

#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

/* Librerie per la generazione casuale */
#include <stdlib.h>
#include <time.h>

/*Librerie per la manipolazione dei processi */
#include <sys/types.h>
#include <sys/wait.h>


#define UP        65 /* Cursore sopra */
#define DW        66 /* Cursore sotto */
#define SX        68 /* Cursore sinistra */
#define DX        67 /* Cursore destra */
#define MAXX      80 /* Dimensione dello schermo di output(colonne) */
#define MAXY      24 /* Dimensione dello schermo di output (righe) */
#define DELAY  20000 /* Ritardo del movimento degli aerei nemici (da adattare) */



/* Struttura adoperata per veicolare le coordinate */
typedef struct{
	int  x; /* Coordinata x */
	int  y; /* Coordinata y */
	char c; /* Identificatore personaggio */
}pos;



/* Firme delle funzioni Invasione Aliena */
void AstronaveMadre (int pipeout);
void DiscoVolante (int pipeout);
void CannoneLaser (int pipeout);
void AreaGioco(int pipein);
void stampaFine(void);


/*
----------------------------------------------------------------------------------------
 Funzione principale programma
----------------------------------------------------------------------------------------
*/

int main(){

int p[2];           /* Filedescriptor -> nome della pipe */
int pidAstronave;   /* Pid processo figlio Astronave Madre */
int pidLaser;       /* Pid processo figlio Cannone Laser */


initscr();         /* Inizializza schermo di output */
noecho();          /* Imposta modalita' da tastiera */
curs_set(0);       /* Nasconde il cursore */

srand(time(NULL)); /* Inizializza seme generatore di passi casuali */


/* Creazione della pipe e controllo errori invocazione */
if(pipe(p)==-1){
	perror("Errore nella creazione della pipe!");
	_exit(1);
}



pidAstronave=fork(); /* Creazione del primo processo figlio Astronave Madre */

switch(pidAstronave){
	case -1: perror("Errore nell'esecuzionde della fork.");
	_exit(2);

	/* Se il pid=0 siamo nel caso del primo figlio Astronave Madre */
	case 0: close(p[0]); /* Chiusura del descrittore di lettura */
		AstronaveMadre(p[1]); /* Il primo processo figlio invoca la funzione dell'Astronave Madre */


	/* Se sono il padre invoco il secondo processo figlio del Cannone Laser */
	default:pidLaser=fork();
		switch(pidLaser){

		/* Controllo errori */
		case -1: perror("Errore nell'esecuzione della fork");
			_exit(3);

		/* Caso del secondo figlio Cannone Laser */

		/* Visualizzo il cannone laser nella posizione iniziale */
		mvprintw(MAXY/2,MAXX/2,"#");

		case 0: close(p[0]); /* Chiusura descr lettura */
		CannoneLaser(p[1]); /* Il secondo processo figlio invoca la f contadino */

		/* Se sono ancora il processo padre */
		default: close(p[1]); /* Chiusura del descrittore di scrittura */
		AreaGioco(p[0]); /* Il processo padre invoca la funzione di controllo */

	}// Fine secondo switch
} // Fine primo switch


/* A questo punto siamo usciti dalla funzione di controllo e vengono terminati i 2 processi figli e ripristinato il normale modo operativo dello schermo */
kill(pidLaser,1);
kill(pidAstronave,1);


/* Termino il gioco ed esco dal programma */
endwin();
stampaFine();
return 0;
}



/*
------------------------------------------------------------------------------------------
 Funzione Astronave Madre
------------------------------------------------------------------------------------------
*/


void AstronaveMadre (int pipeout){

int direction=1;    /* Spostamento orizzontale */
int counter=0, value=0; /* Contatore e intervallo per uscita disco volante */

pos Astronave, Disco;

Astronave.x= 0;   /*Coordinata iniziale X */
Astronave.y= 1;   /* Coordinata iniziale Y */
Astronave.c='=';  /* Carattere Identificativo *



/* Comunico le coordinate iniziali al processo padre */
write(pipeout, &Astronave, sizeof(Astronave));


while(true){

	/* Genero numero casuale tra 10 e 50 */
	value = 10 + random()%40;

	/* Ad intervalli casuali abilita l'uscita di un disco volante */
	if(!(counter++%value)){

	/* Imposto posizione x di partenza e carattere oggetto */
	Disco.x=Astronave.x;
	Disco.c='o';

	/* Effettuo il percorso da y=2 a y=MAXY */
	for(Disco.y=2; Disco.y<=MAXY; Disco.y++){

	/* Comunico le coordinate correnti al processo padre */
	write(pipeout,&Disco,sizeof(Disco));

	/* Inserisco una pausa per rallentare il movimento */
	usleep(DELAY);
	}

	/* Cancello ultima coordinata per evitare collisione */
	Disco.y = -1;
	write(pipeout,&Disco,sizeof(Disco));
	}

	/* Genero coordinate per il moviemnto */
	if(Astronave.x == MAXX-1) direction =-1;
	if(Astronave.x==0)     direction =1;
	Astronave.x = Astronave.x + (direction);


	/* Comunico le coordinate correnti al processo padre*/
	write(pipeout, &Astronave, sizeof(Astronave));
	usleep(DELAY*2); /* Definisce quanto va veloce l'Astronave Madre */

}

}


/*
----------------------------------------------------------------------------------------
 Funzione cannone Laser - Movimento tramite i tasti cursore
----------------------------------------------------------------------------------------
*/

void CannoneLaser (int pipeout){

char c;

pos Laser;
Laser.x = MAXX/2; /* Coordinata X iniziale */
Laser.y = MAXY-1;    /* Coordinata Y iniziale */
Laser.c = '^';    /* Carattere identificativo */


/* Comunico al processo padre le coordiante iniziali */
write(pipeout, &Laser, sizeof(Laser));


/* Lettura dei tasti cursore */
while(true){

	/* Il cannone laser può andare solo a destra e a sinistra  */
	switch(c=getch()){

		/* Sull'asse delle x teniamo conto della dimensione dello sprite */
		case SX: if(Laser.x>0) Laser.x -=1;
		break;

		case DX: if(Laser.x< MAXX-1) Laser.x +=1;
		break;

	}

	/* Comunico al processo padre le coordinate */
	write(pipeout, &Laser, sizeof(Laser));
}

}




/*
-----------------------------------------------------------------------------------------
 Funzione relativa al processo di visualizzazione e controllo
-----------------------------------------------------------------------------------------
*/

void AreaGioco (int pipein){

pos Astronave, Disco, Laser, valore_letto;
int i=0, scudo =3, collision=0;


/* Visualizzo valore scudo iniziale del cannone laser */
	mvprintw(0,1,"Scudo: %d", scudo);


/* Il ciclo si ripete fino a quando collision !=1 */
do{


	/* Leggo dalla pipe */
	read(pipein, &valore_letto, sizeof(valore_letto));

	/* Astronave Madre */
	if(valore_letto.c=='='){

		/* Cancello il precedente carattere visualizzato */
		mvaddch(Astronave.y, Astronave.x,' ');

		/* Aggiorno le coordinate realtive alla nuova posizione */
		Astronave=valore_letto;
	}

	/* Disco volante */
	if(valore_letto.c=='o'){
		/* Cancello il precedente carattere visualizzato */
		mvaddch(Disco.y, Disco.x,' ');

		/* Aggiorno le coordinate realtive alla nuova posizione */
		Disco=valore_letto;
	}

	/* Cannone Laser */
	if(valore_letto.c=='^'){
		/* Cancello il precedente carattere visualizzato */
		mvaddch(Laser.y, Laser.x,' ');

		/* Aggiorno le coordinate realtive alla nuova posizione */
		Laser=valore_letto;
	}


	/* Visualizzo gli oggetti sulle coordinate correnti */
	mvaddch(valore_letto.y, valore_letto.x, valore_letto.c);

	/* Visualizzo valore scudo protettivo */
	mvprintw(0,1,"Scudo: %d", scudo);

	/* Nascondo il cursore */
	curs_set(0);

	/* Aggiorno lo schermo di output per visualizzare le modifiche */
	refresh();

	/* Segnalo collisione CannoneLaser/DiscoVolante, con meccanismo per evitare il rilevamento multiplo della stessa collisine*/
	if(Laser.x == Disco.x && Laser.y == Disco.y){
	if(Laser.x < MAXX) Laser.x++; else Laser.x--;
	mvprintw(Laser.y, Laser.x, "^"); /* Ridisegno cannone laser colpito */
	scudo--;
	}

	/* Esce quando terminano le possibili collisioni */
	if(scudo <1) collision =1;


}while (!collision);

}


/*
-------------------------------------
 Output di fine gioco
-------------------------------------
*/

void stampaFine(void){

 int i;
 char Fine [6][25]={
 " _____ _            ",
 "|  ___(_)_ __   ___ ",
 "| |_  | | '_ \\/ _  \\",
 "|  _| | | | | |  __/",
 "|_|   |_|_| |_|\\___|",
 "                    "
 };

for(i=0; i<6; i++){
printf("%s\n", Fine[i]);

}


printf("\n\n");
}
