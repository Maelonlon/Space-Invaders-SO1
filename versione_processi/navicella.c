#include "progetto.h"

void Navicella(int pipe){

  /*ogni elemento ha un id univoco, percio useremo dei contatori per non rischiare sovrapporre id che potrebbero essere identici, 
  e tenerli in un range gestibile al fine di evitare dei bug*/

  InfoPipe infoNavicella = {15, 2, 0, getpid(), 0};  /*struttura contenente le informazioni della navicella che passeranno attraverso la pipe*/
  int input, pidMissileUp, pidMissileDw, contatoreId = 11;  /*inizializzazione variabili*/

  int contatoreMissili =0; /*contatore missili*/

  time_t orarioCorrente, orarioMissilePrec; /*serve a dare un tempo ai missili*/

  write(pipe,&infoNavicella,sizeof(infoNavicella)); /*informazioni della navicella che vengono prese dalla pipe e scritte*/

  while(1){

    //usleep(DELAY);


    input = getch();

    /*funzione che serve a far muovere la navicella a seconda dell'input dato ma con dei limiti prestabiliti*/
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

      case SPACE:             /*quando viene schiacciato spazio succede quello qui di seguito*/
      time(&orarioCorrente);  /*time prende il tempo e lo salva in orarioCorrente*/
      if(orarioCorrente-orarioMissilePrec>=1){  /*vengono confrontati gli orari del missile sparato precedentemente rispetto a quello che vorremmo sparare*/

        pidMissileUp = fork();   /*creo il processo del missile*/
        switch(pidMissileUp){
          case -1:
            perror("Errore nell'esecuzione della fork Missile");
            exit(2);
          case 0:
            missileUp(pipe, infoNavicella, contatoreId);  /*dati del missile che verranno passati alla pipe*/
            break;
          default:
          //usleep(DELAY/2);
          break;
        }
 
        pidMissileDw = fork(); /*creo il processo del missile*/
        switch(pidMissileDw){
          case -1:
            perror("Errore nell'esecuzione della fork Missile");
            exit(2);
          case 0:
            missileDw(pipe, infoNavicella, contatoreId+1); /*dati del missile che verranno passati alla pipe. (i missili sparati sono due)*/
          default:
          break;
        }

        if(contatoreId >= 79){
          contatoreId = 11; //se il contatore è maggiore o uguale a 79(limite id per i missili) allora ritorna al limite inferiore 11
        }
        contatoreId += 2;//altrimenti il contatore dell'id aumentera di 2(fino a raggiungere il limite superiore)
      }
      orarioMissilePrec = orarioCorrente;//per evitare spam di missili l'orarioCorrente viene settato di volta in volta 

    }

    waitpid(-1, NULL, WNOHANG);
    write(pipe,&infoNavicella,sizeof(infoNavicella)); /*inserisco i dati della navicella nella pipe*/


  }
}

void missileUp (int pipe, InfoPipe infoNavicella, int id){ 
  int counter=0;

  InfoPipe infoMissileUp = {infoNavicella.x+3, infoNavicella.y, id, getpid(), 0};  /*struttura per passare le informazioni del missileUp*/

  write(pipe,&infoMissileUp,sizeof(infoMissileUp));/*informazioni del missile up che vengono scritte nella pipe*/

  while (1) {

    if(counter==8){   //ogni qual volta il counter arriva ad 8 la direzione del missile combiera direzione con y-1
      infoMissileUp.y -=1;
      counter=0;
    } else {
      counter++;
    }

      infoMissileUp.x +=1; //x invece avanza ad ogni turno senza bisogno di un counter

    write(pipe,&infoMissileUp,sizeof(infoMissileUp)); /*informazioni del missile up che vengono scritte nella pipe*/
    usleep(DELAY/2);
  }

}

void missileDw (int pipe, InfoPipe infoNavicella, int id){
  int counter=0;

  InfoPipe infoMissileDw = {infoNavicella.x+3, infoNavicella.y+2, id, getpid(), 0}; /*struttura per passare le informazioni del missileDw*/

  write(pipe,&infoMissileDw,sizeof(infoMissileDw));/*informazioni del missile down che vengono scritte nella pipe*/

  while (1) {

    if(counter==8){//ogni qual volta il counter arriva ad 8 la direzione del missile combiera direzione con y+1
      infoMissileDw.y +=1;   
      counter=0;
    } else {
      counter++;
    }

      infoMissileDw.x +=1;//x invece avanza ad ogni turno senza bisogno di un counter


    write(pipe,&infoMissileDw,sizeof(infoMissileDw)); /*informazioni del missile down che vengono scritte nella pipe*/
    usleep(DELAY/2);
  }

}
