#include "progetto.h"
//funzione del background mobile 
void* Background(void* arg){

  InfoPipe infoBackground = {MAXX , MAXY+1, 0, 'B', 0, pthread_self()};
  int i;


 	bufferIn(infoBackground); //metto il background dentro il buffer
    usleep(DELAY*2);
  while (1) {


    
    usleep(DELAY*2);

bufferIn(infoBackground);

  }
}
