#include "progetto.h"

void Background(int pipe){

  InfoPipe infoBackground = {MAXX , MAXY+1, 999, getpid(), 0}; /*struttura contenente il background*/
  int i;


  write(pipe,&infoBackground,sizeof(infoBackground)); /*inserimento del background dentro la pipe*/


    usleep(DELAY);           /*velocita del background*/


  while (1) {


    write(pipe,&infoBackground,sizeof(infoBackground)); /*inserimento del background dentro la pipe*/
    usleep(DELAY);          /*velocita del background*/



  }
}
