#include "progetto.h"

int main(){


  int i=0;
  int idAlieno;
  posIn = 0;
  posOut = 0;
contatoreIdAlieni=0;
contatoreIdMissili=0;
contatoreIdBombe=0;

  pthread_mutex_init(&mtx, NULL);
  sem_init (&pieno, 0, 0);
  sem_init (&vuoto, 0, DIM_BUFFER);
  srand(time(NULL));



  initscr();
  noecho();
  curs_set(0);

refresh();

/**
 Compito del produttore è generare dati e depositarli nel buffer in continuo.
 Contemporaneamente, il consumatore utilizzerà i dati prodotti, rimuovendoli di volta in volta dal buffer. Il problema è
 assicurare che il produttore non elabori nuovi dati se il buffer è pieno, e che il consumatore non cerchi dati se il buffer è vuoto.
 */
buffer = (InfoPipe*) malloc(sizeof(InfoPipe)*DIM_BUFFER);  //creazione di un buffer condiviso dai thread





if(pthread_create(&tNavicella, NULL, Navicella, NULL))
{
  endwin();
  exit;
}

for(i=0;i<N_ALIENI; i++){

  if(pthread_create(&tAlieno, NULL, Alieno, &i))
  {
    endwin();
    exit;
  }

}

if(pthread_create(&tBackground, NULL, Background, NULL))
{
  endwin();
  exit;
}


  Controllo();


/*
  pthread_join(tNavicella, NULL);
  pthread_join(tBackground, NULL);



  for(i=0;i<N_ALIENI;i++){
    pthread_join(tAlieno, NULL);

  };
*/

  pthread_mutex_destroy (&mtx); //distrugge un mutex
  /*sem_destroy () distrugge il semaforo senza nome all'indirizzo
    indicato dalla sem .*/
sem_destroy (&pieno);
sem_destroy (&vuoto);

  endwin();

  return 0;
}


void bufferIn(InfoPipe info){
  /*sem_wait () decrementa (blocca) il semaforo indicato da sem .
       Se il valore del semaforo è maggiore di zero, il decremento
       procede, e la funzione ritorna, immediatamente. Se la
       il semaforo ha attualmente il valore zero, quindi la chiamata si blocca
       fino a quando non diventa possibile eseguire il decremento (es.
       il valore del semaforo sale sopra lo zero) o un gestore di segnale
       interrompe la chiamata.*/

  sem_wait(&vuoto); //semaforo
  pthread_mutex_lock(&mtx);//semaforo sulla sezione critica che garantisce la mutua esclusivita (chiusura semaforo)
  buffer[posIn] = info;//array delle informazioni relative all'oggetto che entra nella funzione
  posIn = (posIn+1)%DIM_BUFFER; //serve a scorrere le informazioni da depositare nel buffer ciclico
  pthread_mutex_unlock(&mtx);//semaforo sulla sezione critica che garantisce la mutua esclusivita (apertura semaforo)
  sem_post(&pieno);
}

InfoPipe bufferOut(){
  InfoPipe out;

  sem_wait(&pieno);
  pthread_mutex_lock(&mtx);
  out = buffer[posOut];  //informazioni,relative all'oggetto, che escono dal buffer 
  posOut = (posOut+1)%DIM_BUFFER;
  pthread_mutex_unlock(&mtx);
  sem_post(&vuoto);
  return out;
}


void setArgs(InfoPipe info, int id, int* args){
  //x,y,id.
  args[0] = info.x;
  args[1] = info.y;
  args[2] = id;
}
