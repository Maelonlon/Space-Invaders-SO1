#include "progetto.h"




int main(){

  int pidNavicella, pidAlieno[N_ALIENI], pidControllo, pidBackground, i=0;

  int p[2];

  int status;

  srand(time(NULL));


  pipe(p);

  initscr();
  noecho();
  curs_set(0);


refresh();

  pidNavicella = fork(); /*creo il processo della navicella, che avra il suo pid*/
  switch(pidNavicella){
    case -1:
      perror("Errore nell'esecuzione della fork Missile");
      exit(2);
    case 0:
      Navicella(p[1]);
      break;
    default:
    //usleep(DELAY/2);
    break;
  }

  //  waitpid(pidNavicella, &status);


  pidBackground = fork(); /*creo la fork del background*/
  switch(pidBackground){
    case -1:
      perror("Errore nell'esecuzione della fork Missile");
      exit(2);
    case 0:
      Background(p[1]);
      break;
    default:

    break;
  }
  //waitpid(pidBackground, &status);



  for(i=0;i<N_ALIENI; i++){

    pidAlieno[i] = fork(); /*creo tutte le i-esime fork degli alieni*/
    switch(pidAlieno[i]){
      case -1:
        perror("Errore nell'esecuzione della fork Missile");
        exit(2);
      case 0:
        Alieno(p[1], i+1);
        break;
      default:

      break;
    }

    //waitpid(pidAlieno[i], &status);


  }







  pidControllo = fork(); /*creo il processo di controllo*/
  switch(pidControllo){
    case -1:
      perror("Errore nell'esecuzione della fork Missile");
      exit(2);
    case 0:
      Controllo(p[0], pidAlieno);
      break;
    default:
    //usleep(DELAY/2);
    break;
  }
  //waitpid(pidControllo, &status);

  //waitpid(pidControllo, NULL, 0);


  waitpid(pidControllo, NULL, 0);


  for(i=0;i<N_ALIENI;i++){
    kill(pidAlieno[i], 1);

  };
  kill(pidNavicella,1); /*serve a terminare il processo della navicella riconoscendola tramite il suo pid*/

  kill(pidBackground, 1);/*serve a terminare il processo del background riconoscendolo tramite il suo pid*/

  kill(pidControllo, 1);/*serve a terminare il processo di controllo riconoscendolo tramite il suo pid*/
  //waitpid(pidBackground, NULL, 0);



  endwin();

  printf("GAME OVER\n");
  exit(0);

  return 0;
}
