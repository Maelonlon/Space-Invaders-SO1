#include "progetto.h"

void* Navicella(void* arg){

  int input, i, contatoreId = 0;
  time_t orarioCorrente, orarioMissilePrec;
  InfoPipe infoNavicella; //struttura contenente tutti i dati della navicella

  infoNavicella.x=15;
  infoNavicella.y=2;
  infoNavicella.morto=0;
  infoNavicella.type='n';
  infoNavicella.id=0;
  infoNavicella.tid = pthread_self(); //id del thread della navicella

  bufferIn(infoNavicella);

  while(1){

    //usleep(DELAY);


    input = getch();

    /* 
    input che servono a far interagire l'utente nel gioco tramite la navicella (spostamenti, spari)
    */
    switch (input) {
      case UP:
        if (infoNavicella.y > 0){
          infoNavicella.y-=1;
        }
        break;

      case DW:
        if (infoNavicella.y < MAXY - 1){
          infoNavicella.y+=1;
        }
        break;

      case SPACE:

      time(&orarioCorrente);
      //questo serve per non far spammare missilii da parte del giocatore
      if(orarioCorrente-orarioMissilePrec>=1){

        if(contatoreId >= 30){
          contatoreId = 0;
        }

        //creazione del thread attinente ai missili superiori
        if(pthread_create(&tMissileUp, NULL, missileUp, &infoNavicella))
        {
          endwin();
          exit;
        }

        //creazione del thread attinente ai missili inferiori
        if(pthread_create(&tMissileDw, NULL, missileDw, &infoNavicella))
        {
          endwin();
          exit;
        }

        contatoreId++;

      }
      orarioMissilePrec = orarioCorrente;

    }
    bufferIn(infoNavicella);

  }

}

void* missileUp (void* arg){  //void* arg è infoNavicella
  int counter=0, id;

  InfoPipe infoMissileUp, infoNavicella;
infoNavicella = *((InfoPipe*) arg); 

pthread_mutex_lock(&mtx);

  //funzione che aggiorna l'id di ogni singolo missile che viene lanciato
	id = contatoreIdMissili;
	contatoreIdMissili++;
	if(contatoreIdMissili>50){
		contatoreIdMissili=0;
	}

  pthread_mutex_unlock(&mtx);


  infoMissileUp.x = infoNavicella.x+3;
  infoMissileUp.y = infoNavicella.y-1;
  infoMissileUp.morto = 0;
  infoMissileUp.type='m';
  infoMissileUp.id = id;
  infoMissileUp.tid = pthread_self();


  bufferIn(infoMissileUp); //inserisco le informazioni di missileUp nel thread 

  while (1) {

    //movimenti in diagonale del missile up 
    if(counter==8){
      infoMissileUp.y -=1;
      counter=0;
    } else {
      counter++;
    }

      infoMissileUp.x +=1;

    bufferIn(infoMissileUp);


    usleep(DELAY/2);
  }

  pthread_exit(0);
}

void* missileDw (void* arg){ //void* arg è infoNavicella
  int counter=0, id;
    InfoPipe infoMissileDw, infoNavicella;

infoNavicella = *((InfoPipe*) arg);

pthread_mutex_lock(&mtx);

  //contatore ciclico degli id dei missili
	id = contatoreIdMissili;
	contatoreIdMissili++;
	if(contatoreIdMissili>=60){
		contatoreIdMissili=0;
	}

  pthread_mutex_unlock(&mtx);
 
  //informazioni del missileDw
  infoMissileDw.x = infoNavicella.x+3;
  infoMissileDw.y = infoNavicella.y+1;
  infoMissileDw.morto = 0;
  infoMissileDw.type='m';
  infoMissileDw.id = id;
  infoMissileDw.tid = pthread_self();

  bufferIn(infoMissileDw);//inserimento delle informazioni del missile Dw nel buffer

  while (1) {

    //movimento diagonale verso il basso del missile inferiore
    if(counter==8){
      infoMissileDw.y +=1;
      counter=0;
    } else {
      counter++;
    }

      infoMissileDw.x +=1;

      bufferIn(infoMissileDw); //inserimento nel buffer del missileDw

    usleep(DELAY/2); //velocita del missile
  }

  pthread_exit(0); /*chiude il thread e restituisce un valore,Per consentire ad altri thread di continuare l'esecuzione, il thread principale
                      dovrebbe terminare chiamando pthread_exit () */
}
