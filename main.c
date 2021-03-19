#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
/*Librerie per la manipolazione dei processi */
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define LIVELLO
#define VITE

#define UP        65 /* Cursore sopra */
#define DW        66 /* Cursore sotto */
#define SX        68 /* Cursore sinistra */
#define DX        67 /* Cursore destra */
#define SPACE			32
#define MAXX      80 /* Dimensione dello schermo di output(colonne) */
#define MAXY      24 /* Dimensione dello schermo di output (righe) */
#define DELAY  100000 /* Ritardo del movimento delle navicelle nemiche (da adattare) */

/* Struttura adoperata per veicolare le coordinate */
typedef struct{
	int  x; /* Coordinata x */
	int  y; /* Coordinata y */
	char c[3]; /* Identificatore personaggio sprite*/
}pos;

typedef struct{
	int  x; /* Coordinata x */
	int  y; /* Coordinata y */
	char c; /* Identificatore personaggio sprite*/
}pos_bullet;

void Nemici(int pipeout);
void Amici(int* p);
void AreaGioco(int pipein);
void MissileDx(int pipein, pos Amici);
void MissileSx(int pipein, pos Amici);

int main(){

  int p[2];           /* Filedescriptor -> nome della pipe */
  int pidAmici;   /* Pid processo figlio Astronave */
	int pidNemici;		//Pid processo figlio Navicelle Nemiche

  initscr();         /* Inizializza schermo di output */
  noecho();          /* Imposta modalita' da tastiera */
  curs_set(0);       /* Nasconde il cursore */

  srand(time(NULL)); /* Inizializza seme generatore di passi casuali */

  /* Creazione della pipe e controllo errori invocazione */
if(pipe(p)==-1){
	perror("Errore nella creazione della pipe!");
	_exit(1);
}

pidAmici=fork(); /* Creazione del primo processo figlio Astronave Madre */

switch(pidAmici){
  case -1:
    perror("Errore nell'esecuzione della fork Amici");
    _exit(2);
  case 0:
    close(p[0]);
    Amici(p);
  default:
		pidNemici=fork();

		switch(pidNemici){
			case -1:
    		perror("Errore nell'esecuzione della fork Nemici");
    		_exit(3);
  		case 0:
    		close(p[0]);
    		Nemici(p[1]);
  		default:
				close(p[1]);
				AreaGioco(p[0]);
		}
}

kill(pidNemici, 1);
kill(pidAmici, 1);

}

void Amici (int* p){

	int pidMissileDx, pidMissileSx;

	char c;

	pos Amici;

	Amici.x= MAXX/2;   /*Coordinata iniziale X */
	Amici.y= MAXY-1;   /* Coordinata iniziale Y */
	//Amici.c= "o^o";

	strcpy(Amici.c, "<O>");  /* Carattere Identificativo */


	int dimSprite = sizeof(Amici.c);

	/* Comunico le coordinate iniziali al processo padre */
	close(p[0]);
	write(p[1], &Amici, sizeof(Amici));


	/* Lettura dei tasti cursore */
	while(true){



		/* Il cannone laser può andare solo a destra e a sinistra  */
		switch(c=getch()){

			/* Sull'asse delle x teniamo conto della dimensione dello sprite */
			case SX: if(Amici.x>0) Amici.x -=1;
			break;

			case DX: if(Amici.x< MAXX-dimSprite) Amici.x +=1;
			break;

			case SPACE:

				pidMissileDx = fork();
				switch(pidMissileDx){
					case -1:
						perror("Errore nell'esecuzione della fork Missile");
						_exit(2);
					case 0:
					close(p[0]);
						MissileDx(p[1], Amici);
						break;
					default:
					pidMissileSx = fork();

					switch(pidMissileSx){
						case -1:
							perror("Errore nell'esecuzione della fork Missile");
							_exit(2);
						case 0:
						close(p[0]);
							MissileSx(p[1], Amici);
						default:
							break;
				}
				}
			}

		/* Comunico al processo padre le coordinate */
		close(p[0]);
		write(p[1], &Amici, sizeof(Amici));
	}

}

void MissileSx (int pipeout, pos Amici){

	pos Missile;

	//while (true){

		strcpy(Missile.c, " \\ ");
		//Missile.x = Amici.x;
		//Missile.y = Amici.y-1;

		//for(Missile.y=Amici.y-1, Missile.x=Amici.x; Missile.y>=2; Missile.y-- && Missile.x--){
		Missile.x = Amici.x;
		for(Missile.y=Amici.y-1; Missile.y>=1; Missile.y--){
		/* Comunico le coordinate correnti al processo padre */
		write(pipeout,&Missile,sizeof(Missile));

		/* Inserisco una pausa per rallentare il movimento */
		usleep(DELAY);
		Missile.x--;
		}

		/* Cancello ultima coordinata per evitare collisione */
		Missile.y = -1;
		write(pipeout,&Missile,sizeof(Missile));


	//}
}

void MissileDx (int pipeout, pos Amici){

	pos Missile;

	//while (true){

		strcpy(Missile.c, " / ");
		//Missile.x = Amici.x;
		//Missile.y = Amici.y-1;

		//for(Missile.y=Amici.y-1, Missile.x=Amici.x+1; Missile.y>=2; Missile.y-- && Missile.x++){
		Missile.x = Amici.x+1;
		for(Missile.y=Amici.y-1; Missile.y>=1; Missile.y--){

		/* Comunico le coordinate correnti al processo padre */
		write(pipeout,&Missile,sizeof(Missile));

		/* Inserisco una pausa per rallentare il movimento */
		usleep(DELAY);
		Missile.x++;
		}

		/* Cancello ultima coordinata per evitare collisione */
		Missile.y = -1;
		write(pipeout,&Missile,sizeof(Missile));

	//}
}

void Nemici (int pipeout){

int direction=1;    /* Spostamento orizzontale */
int counter=0, value=0; /* Contatore e intervallo per uscita disco volante */

pos Nemici;
pos N_bullet;

Nemici.x= 0;   /*Coordinata iniziale X */
Nemici.y= 1;   /* Coordinata iniziale Y */
//Astronave.c='=';  /* Carattere Identificativo */

strcpy(Nemici.c, "vOv");

int dimSprite = sizeof(Nemici.c);


/* Comunico le coordinate iniziali al processo padre */
write(pipeout, &Nemici, sizeof(Nemici));


while(true){

	/* Genero numero casuale tra 10 e 50 */
	value = 10 + random()%40;

	/* Ad intervalli casuali abilita l'uscita di un disco volante */
	if(!(counter++%value)){

	/* Imposto posizione x di partenza e carattere oggetto */
	N_bullet.x=Nemici.x+1;
	strcpy(N_bullet.c, " o ");


	/* Effettuo il percorso da y=2 a y=MAXY */
	for(N_bullet.y=2; N_bullet.y<=MAXY; N_bullet.y++){

	/* Comunico le coordinate correnti al processo padre */
	write(pipeout,&N_bullet,sizeof(N_bullet));

	/* Inserisco una pausa per rallentare il movimento */
	usleep(DELAY);
	}

	/* Cancello ultima coordinata per evitare collisione */
	N_bullet.y = -1;
	write(pipeout,&N_bullet,sizeof(N_bullet));
	}

	/* Genero coordinate per il moviemnto */
	if(Nemici.x >= MAXX-dimSprite) direction =-1;
	if(Nemici.x==0)     direction =1;
	Nemici.x = Nemici.x + (direction);


	/* Comunico le coordinate correnti al processo padre*/
	write(pipeout, &Nemici, sizeof(Nemici));
	usleep(DELAY*2); /* Definisce quanto va veloce l'Astronave Madre */

	}
}


void AreaGioco (int pipein){

pos Amici, MissileDx, MissileSx, Nemici, N_bullet, valore_letto;
int i=0, scudo =3, collision=0;


/* Visualizzo valore scudo iniziale del cannone laser */
	mvprintw(0,1,"Scudo: %d", scudo);


/* Il ciclo si ripete fino a quando collision !=1 */
do{


	/* Leggo dalla pipe */
	read(pipein, &valore_letto, sizeof(valore_letto));

	/* Astronave Madre */
	if(strcmp(valore_letto.c, "vOv") == 0){

		/* Cancello il precedente carattere visualizzato */
		mvprintw(Nemici.y, Nemici.x,"   ");

		/* Aggiorno le coordinate realtive alla nuova posizione */
		Nemici=valore_letto;
	}

	/* Disco volante */
	if(strcmp(valore_letto.c, " o ") == 0){
		/* Cancello il precedente carattere visualizzato */
		mvprintw(N_bullet.y, N_bullet.x,"   ");

		/* Aggiorno le coordinate realtive alla nuova posizione */
		N_bullet=valore_letto;
	}

	/* Cannone Laser */
	if(strcmp(valore_letto.c, "<O>") == 0){
		/* Cancello il precedente carattere visualizzato */
		mvprintw(Amici.y, Amici.x,"   ");

		/* Aggiorno le coordinate realtive alla nuova posizione */
		Amici=valore_letto;
	}

	if(strcmp(valore_letto.c, " / ") == 0){

		mvprintw(MissileDx.y, MissileDx.x, "   ");

		MissileDx = valore_letto;
	}

	if(strcmp(valore_letto.c, " \\ ") == 0){

		mvprintw(MissileSx.y, MissileSx.x, "   ");

		MissileSx = valore_letto;
	}


	/* Visualizzo gli oggetti suDiscolle coordinate correnti */
	mvprintw(valore_letto.y, valore_letto.x, valore_letto.c);

	/* Visualizzo valore scudo protettivo */
	mvprintw(0,1,"Scudo: %d", scudo);

	/* Nascondo il cursore */
	curs_set(0);

	/* Aggiorno lo schermo di output per visualizzare le modifiche */
	refresh();

	/* Segnalo collisione CannoneLaser/DiscoVolante, con meccanismo per evitare il rilevamento multiplo della stessa collisine*/
	//if(Laser.x == Disco.x && Laser.y == Disco.y){
	//if(Laser.x < MAXX) Laser.x++; else Laser.x--;
	//mvprintw(Laser.y, Laser.x, "^"); /* Ridisegno cannone laser colpito */
	//scudo--;
	//}

	/* Esce quando terminano le possibili collisioni */
	//if(scudo <1) collision =1;


	}while (!collision);

	}

/*
-------------------------------------
 Output di fine gioco
-------------------------------------
*/
