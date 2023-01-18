#include "progetto.h"

void* Alieno(void* arg){


  char direzione = 'd';
  int pidBomba, contatoreId, attivaBomba=0, bombaCasuale, idAlieno;
  InfoPipe infoAlieno;
  idAlieno = *((int*) arg);
  contatoreId = idAlieno;

  pthread_mutex_lock(&mtx); 
  //contatore dell'id degli alieni
	idAlieno = contatoreIdAlieni;
	contatoreIdAlieni++;
  pthread_mutex_unlock(&mtx);


  usleep(DELAY*(20*(idAlieno))); //serve per creare gli alieni con un delay tale da non farli uscire tutti insieme
  //informazioni dell'alieno
  infoAlieno.x=120;
  infoAlieno.y=2;
  infoAlieno.morto=0;
  infoAlieno.type = 'a';
  infoAlieno.id=idAlieno;
  infoAlieno.tid = pthread_self();

  bufferIn(infoAlieno); //informazioni dell'alieno inserite dentro il buffer

  while (1) {
    //movimenti degli alieni
    if(direzione == 'd'){
      infoAlieno.y +=1;
    } else {
      infoAlieno.y -=1;
    }

    if(infoAlieno.y <= 0 ){
      direzione = 'd';
      infoAlieno.x -= 4 ;
    }
    if(infoAlieno.y >= MAXY-1 ){
      direzione = 'u';
      infoAlieno.x -= 4 ;
    }


    //creazione di bombe che escono in un momento casuale da ogni alieno
    bombaCasuale = rand() % (48+idAlieno - 1);

    if(bombaCasuale==0){

      if(contatoreId> 50){
        contatoreId=idAlieno; //contatore id degli alieni
      }

      if(pthread_create(&tBomba, NULL, bomba, &infoAlieno)) //thread della bomba lanciata dall'alieno
      {
        endwin();
        exit;
      }

      contatoreId+=N_ALIENI;

    }
    usleep(DELAY*2);
    bufferIn(infoAlieno); 

    }
    pthread_exit(0);
  }


void* bomba(void* arg){
  InfoPipe infoBomba, infoAlieno;
	 infoAlieno = *((InfoPipe*) arg);
	int id;


 pthread_mutex_lock(&mtx);
  //contatore delle bombe lanciate da ogni alieno
	id = contatoreIdBombe;
	contatoreIdBombe++;
	if(contatoreIdBombe>=50){
		contatoreIdBombe=0;
	}

  pthread_mutex_unlock(&mtx);
    //informazioni attinenti alle bombe
    infoBomba.x = infoAlieno.x-1;
    infoBomba.y = infoAlieno.y;
    infoBomba.id = id;
    infoBomba.type = 'b';
    infoBomba.morto = 0;
    infoBomba.tid = pthread_self();

    bufferIn(infoBomba);//metto le informazioni della bomba dentro il buffer


  while (1) {

      infoBomba.x -=1;//aggiorno il movimento della bomba

    usleep(DELAY);
    bufferIn(infoBomba);//metto le informazioni della bomba dentro il buffer
  }
  pthread_exit(0);

}
