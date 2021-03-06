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
#define DELAY  50000 /* Ritardo del movimento delle navicelle nemiche (da adattare) */
#define N_NEMICI	 10

/* Struttura adoperata per veicolare le coordinate */
typedef struct{
	int  x; /* Coordinata x */
	int  y; /* Coordinata y */
	int	id;
	char s[4];  //sprite
	char c;			//bullet
}pos;

void Nemici(int* p, int id);
void Amici(int* p);
void AreaGioco(int pipein);
void MissileDx(int pipeout, pos Amici);
void MissileSx(int pipeout, pos Amici);
void Bomba(int pipeout, pos Nemici);

int main(){

  int p[2];           /* Filedescriptor -> nome della pipe */
  int pidAmici;   /* Pid processo figlio Astronave */
	int pidNemici[N_NEMICI];		//Pid processo figlio Navicelle Nemiche
	int i;

  initscr();         /* Inizializza schermo di output */
  noecho();          /* Imposta modalita' da tastiera */
  curs_set(0);       /* Nasconde il cursore */

  srand(time(NULL)); /* Inizializza seme generatore di passi casuali */

  /* Creazione della pipe e controllo errori invocazione */
if(pipe(p)==-1){
	perror("Errore nella creazione della pipe!");
	_exit(1);
}

 /* Creazione del primo processo figlio Astronave Madre */

switch(pidAmici=fork()){
  case -1:
    perror("Errore nell'esecuzione della fork Amici");
    _exit(2);
  case 0:
    close(p[0]);
    Amici(p);
		_exit(0);
  default:
	break;
}

for(i=0; i<N_NEMICI; i++){
	usleep(DELAY*10);

	switch(pidNemici[i]=fork()){
		case -1:
  		perror("Errore nell'esecuzione della fork Nemici");
  		_exit(3);
		case 0:
  		close(p[0]);
  		Nemici(p, i+1);
			_exit(0);
		default:
			break;
	}
}


close(p[1]);
AreaGioco(p[0]);

for(i=0; i<N_NEMICI; i++){
kill(pidNemici[i], 1);
}
kill(pidAmici, 1);

}

void Amici (int* p){

	int pidMissileDx, pidMissileSx;

	char c;

	pos Amici;

	Amici.x= MAXX/2;   /*Coordinata iniziale X */
	Amici.y= MAXY-1;   /* Coordinata iniziale Y */
	//Amici.c= "o^o";

	strcpy(Amici.s, "<O>");  /* Carattere Identificativo */
	Amici.c = ' ';


	int dimSprite = sizeof(Amici.c);

	/* Comunico le coordinate iniziali al processo padre */
	close(p[0]);
	write(p[1], &Amici, sizeof(Amici));


	/* Lettura dei tasti cursore */
	while(true){



		/* Il cannone laser pu?? andare solo a destra e a sinistra  */
		switch(c=getch()){

			/* Sull'asse delle x teniamo conto della dimensione dello sprite */
			case UP: if(Amici.y>3) Amici.y -=1;
			break;

			case DW: if (Amici.y<MAXY-1) Amici.y +=1;
			break;

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
						pidMissileDx = getpid();
						kill(pidMissileDx, 1);
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
							pidMissileSx = getpid();
							kill(pidMissileSx, 1);
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

		Missile.c = '\\';
		Missile.s[0] = ' ';
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

		Missile.c = '/';
		Missile.s[0] = ' ';
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

void Nemici (int *p, int id){

int direction=1;    /* Spostamento orizzontale */
int counter=0, value=0; /* Contatore e intervallo per uscita disco volante */
int pidBomba;

pos Nemici;
//pos Bomba;

Nemici.x= 1;   /*Coordinata iniziale X */
Nemici.y= 1;   /* Coordinata iniziale Y */
Nemici.id=id;
//Astronave.c='=';  /* Carattere Identificativo */

strcpy(Nemici.s, "vOv");
Nemici.c = ' ';

int dimSprite = sizeof(Nemici.c);


/* Comunico le coordinate iniziali al processo padre */

write(p[1], &Nemici, sizeof(Nemici));


while(true){

	/* Genero numero casuale tra 10 e 50 */
	value = 10 + random()%40;

	/* Ad intervalli casuali abilita l'uscita di un disco volante */
	if(!(counter++%value)){

		pidBomba = fork();
		switch(pidBomba){
			case -1:
				perror("Errore nell'esecuzione della fork Bomba");
				_exit(2);
			case 0:
				close(p[0]);
				Bomba(p[1], Nemici);
				pidBomba = getpid();
				kill(pidBomba, 1);
				break;
			default:
			break;
		}
	}


	/* Genero coordinate per il moviemnto */
	if(Nemici.x >= MAXX-dimSprite){
		Nemici.y++;
		direction =-1;
	}
	if(Nemici.x==0){
		Nemici.y++;
		direction =1;
	}
	Nemici.x = Nemici.x + (direction);


	/* Comunico le coordinate correnti al processo padre*/
	write(p[1], &Nemici, sizeof(Nemici));
	usleep(DELAY*2); /* Definisce quanto va veloce l'Astronave Madre */


}
}

void Bomba (int pipeout, pos Nemici){

	pos Bomba;

	/* Imposto posizione x di partenza e carattere oggetto */
	Bomba.x=Nemici.x+1;
	Bomba.id=Nemici.id;
	Bomba.c = 'o';
	Bomba.s[0] = ' ';


	/* Effettuo il percorso da y=2 a y=MAXY */
	for(Bomba.y=Nemici.y+1; Bomba.y<=MAXY; Bomba.y++){

	/* Comunico le coordinate correnti al processo padre */
	write(pipeout,&Bomba,sizeof(Bomba));

	/* Inserisco una pausa per rallentare il movimento */
	usleep(DELAY);
	}

	/* Cancello ultima coordinata per evitare collisione */
	Bomba.y = -1;
	write(pipeout,&Bomba,sizeof(Bomba));
	}




void AreaGioco (int pipein){

pos Amici, MissileDx, MissileSx, Nemici[N_NEMICI], Bomba[N_NEMICI], valore_letto;
int i=0, scudo =3, collision=0;


/* Visualizzo valore scudo iniziale del cannone laser */
	mvprintw(0,1,"Scudo: %d", scudo);


/* Il ciclo si ripete fino a quando collision 	!=1 */
do{


	/* Leggo dalla pipe */
	read(pipein, &valore_letto, sizeof(valore_letto));

	/* Astronave Madre */
	for(i=0; i<N_NEMICI; i++){
		if(strcmp(valore_letto.s, "vOv") == 0 && valore_letto.id == i+1){

		//Cancello il precedente carattere visualizzato
			mvprintw(Nemici[i].y, Nemici[i].x,"   ");

		// Aggiorno le coordinate realtive alla nuova posizione
			Nemici[i]=valore_letto;
		}

	}



	// Bomba
	for(i=0; i<N_NEMICI+1; i++){
		if(valore_letto.c == 'o'  && valore_letto.id == i+1){
			// Cancello il precedente carattere visualizzato
			mvaddch(Bomba[i].y, Bomba[i].x,' ');


			// Aggiorno le coordinate realtive alla nuova posizione
			Bomba[i]=valore_letto;
		}
	}

	/* Cannone Laser */
	if(strcmp(valore_letto.s, "<O>") == 0){
		/* Cancello il precedente carattere visualizzato */
		mvprintw(Amici.y, Amici.x,"   ");

		/* Aggiorno le coordinate realtive alla nuova posizione */
		Amici=valore_letto;
	}

	if(valore_letto.c == '/'){

		mvaddch(MissileDx.y, MissileDx.x, ' ');

		MissileDx = valore_letto;
	}

	if(valore_letto.c == '\\'){

		mvaddch(MissileSx.y, MissileSx.x, ' ');

		MissileSx = valore_letto;
	}


	/* Visualizzo gli oggetti suDiscolle coordinate correnti */
	if(valore_letto.s[0] != ' '){
		mvprintw(valore_letto.y, valore_letto.x, valore_letto.s);
	} else {
		mvaddch(valore_letto.y, valore_letto.x, valore_letto.c);
	}

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
