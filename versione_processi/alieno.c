#include "progetto.h"

void Alieno(int pipe, int id){



  InfoPipe infoAlieno = {120 , 2, id, getpid(), 0}; /*struttura dell'alieno che deve passare attraverso la pipe*/
  char direzione = 'd';
  int pidBomba, contatoreId= 100+(id-1), attivaBomba=0, bombaCasuale;

  usleep(DELAY*(20*(id-1)));

  write(pipe,&infoAlieno,sizeof(infoAlieno)); /*informazioni dell'alieno che vengono inserite nella pipe*/

  /*funzione dedicata all'avanzare dell'alieno*/
  while (1) {
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

    waitpid(-1, NULL, WNOHANG);
    write(pipe,&infoAlieno,sizeof(infoAlieno));
    usleep(DELAY/2);

    bombaCasuale = rand() % (48+id - 1);    /*bomba che viene sparata in un momento casuale*/

    if(bombaCasuale==0){


      pidBomba = fork();
      switch(pidBomba){
        case -1:
          perror("Errore nell'esecuzione della fork Missile");
          exit(2);
        case 0:
          bomba(pipe, infoAlieno, contatoreId);  /*struttura contenente le informazioni della bomba*/
          break;
        default:
        //usleep(DELAY/2);
        break;
      }
      if(contatoreId> 150){
        contatoreId=100+(id-1);
      }
      contatoreId+=N_ALIENI;        /*contatore id degli alieni, utile per evitare che si creino degli alieni con lo stesso id*/

    }

    }

  }


void bomba(int pipe, InfoPipe info, int id){
  InfoPipe infoBomba = {info.x-1, info.y, id, getpid(), 0};  /*struttura contenente le informazioni della bomba da passare nella pipe*/

  write(pipe,&infoBomba,sizeof(infoBomba)); /*qui passiamo le informazioni della bomba alla pipe*/

  while (1) {

      infoBomba.x -=1;

    write(pipe,&infoBomba,sizeof(infoBomba));
    usleep(DELAY/2);
  }
}
