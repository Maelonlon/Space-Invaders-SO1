#include "progetto.h"

void Controllo(int pipe, int pidAlieno[]){  /*qui gestiremo le collisioni e la stampa degli elementi*/

  InfoPipe info, infoNav, infoAli[N_ALIENI], infoMiss[69], infoBomba[50]; /*strutture di tipo infopipe*/

  /*info serve ad effettuare confronti con le altri strutture sottoriportate
    infoNav serve per ogni interazione con la navicella
    infoAli serve per ogni interazione con gli alieni
    infoMiss serve per ogni interazione con i missili lanciati
    infoBomba serve per ogni interazione con le bombe nemiche
    */

  int i,j,n=0, vite=VITE, viteAlieno[N_ALIENI]={0};
  int status;
  int endgame=0;

  char background[MAXX], backgroundVuoto[MAXX];    /*inizializiamo il background vuoto che poi verra riempito*/

  strcpy(background, generaBackground());           /*tramite le apposite funzioni generiamo il background e lo inseriamo appunto in background*/
  strcpy(backgroundVuoto, generaBackgroundVuoto());

  infoNav.morto=0;

  for(i=0;i<69;i++){
    infoMiss[i].pid=-1;
  }

  for(i=0;i<50;i++){
    infoBomba[i].pid=-1;

  }


  while(endgame==0){

    read(pipe, &info, sizeof(info));



    //COLLISIONI

    if(info.id>0 && info.id<=10){     //se info.id è compreso tra 1 e 10

      if(info.x<=infoNav.x+2){        //se info.x è minore o uguale a infoNav.x+2(vuol dire che la navicella è sovrapposta ad info.x)
                                      //killa infoNav.pid e infoPid

        kill(info.pid,1);             
        info.morto=1;

        kill(infoNav.pid,1);
        infoNav.morto=1;
        cancella3x3(infoNav);
      }
    }

    if(info.id>10 && info.id<=80){                                  /*se info.id è compreso tra 11 e 80 allora: */

      for(i=0;i<N_ALIENI;i++){
        if(infoAli[i].x+1==info.x && infoAli[i].y==info.y ||
           infoAli[i].x==info.x && infoAli[i].y+1==info.y ||
           infoAli[i].x+1==info.x && infoAli[i].y+2==info.y){

          cancella3x3(infoAli[i]);
          /* se le coordinate x,y di infoali sono le stesse di info allora cancella infoAli */

          viteAlieno[i]++;

          if(viteAlieno[i]>=VITE_ALIENO){
            kill(infoAli[i].pid, 1);
            infoAli[i].x=MAXX;
            infoAli[i].y=MAXY;
            infoAli[i].morto=1;

            kill(info.pid,1);
            info.pid=-1;
            //waitpid(info.pid, &status, 0);
            wait(&status);

            info.morto=1;
          }
        }
      }
      if(info.x >= MAXX-1){
        kill(info.pid,1);
        info.pid=-1;



        info.morto=1;
      }

    }

    if(info.id>99 && info.id<=150){

      /*punti di contatto tra la navicella e le bombe nemiche*/
      if(infoNav.x+1==info.x && infoNav.y==info.y ||
         infoNav.x+2==info.x && infoNav.y+1==info.y ||
         infoNav.x+1==info.x && infoNav.y+2==info.y){
        vite--;
        if(vite<=0){
          kill(infoNav.pid, 1);  /*se le vite sono zero allora termina il processo della nostra navicella*/
          infoNav.morto=1;


          cancella3x3(infoNav);
        }

        kill(info.pid,1);

        info.pid=-1;
        info.morto=1;
      }

      if(info.x <= 0){
        kill(info.pid,1);

        info.pid=-1;
        info.morto=1;
      }

    }

    //STAMPA

    if(info.id==0){
      cancella3x3(infoNav);
      infoNav=info;

        if(!info.morto){
          switch(vite){
            case 1:
            stampaNavicella2(info);

            break;
            case 2:
            stampaNavicella1(info);

            break;
            case 3:
            stampaNavicella(info);

            break;
          }

    }

    }

    if(info.id>0 && info.id<=10){

        cancella3x3(infoAli[info.id-1]);
        infoAli[info.id-1]=info;

        if(!info.morto){
          if(viteAlieno[info.id-1]==0){
            stampaAlienolv1(info);
          } else if(viteAlieno[info.id-1]==1){
            stampaAlienolv2(info);
          } else {
            stampaAlienolv2Half(info);

        }
      }
    }

    if(info.id>10 && info.id<=40){
      if
      mvaddch(infoMiss[info.id-11].y, infoMiss[info.id-11].x, ' ');
      infoMiss[info.id-11]=info;

      if(!info.morto){
        mvaddch(infoMiss[info.id-11].y, infoMiss[info.id-11].x, '-');
      }
    }

    if(info.id>41 && info.id<=80){
      mvaddch(infoMiss[info.id-11].y, infoMiss[info.id-11].x, ' ');
      infoMiss[info.id-11]=info;

      if(!info.morto){
        mvaddch(infoMiss[info.id-11].y, infoMiss[info.id-11].x, '-');
      }
    }

    if(info.id>=100 && info.id<=150){
      mvaddch(infoBomba[info.id-100].y, infoBomba[info.id-100].x, ' ');
      infoBomba[info.id-100]=info;

      if(!info.morto){
      mvaddch(infoBomba[info.id-100].y, infoBomba[info.id-100].x, 'o');
      }
    }

    if(info.id==999){

      strcpy(background, spostaBackground(background));
      stampaBackground(backgroundVuoto);
      stampaBackground(background);

    }


    endgame = endgameCheck(infoAli, infoNav);



    refresh();

  }
  for(i=0;i<69;i++){
    if(infoMiss[i].pid>0){
    kill(infoMiss[i].pid, 1);
  }
  }
  for(i=0;i<50;i++){
    if(infoBomba[i].pid>0){
    kill(infoBomba[i].pid, 1);
    }
  }

  clear();
  refresh();
  kill(getpid(),1);
}

int endgameCheck (InfoPipe* infoAli, InfoPipe infoNav){ /*funzione che determina le condizioni di fine gioco*/
  int nAlieniMorti=0, i;

  if(infoNav.morto==1){
    return 1;
  }
  for(i=0;i<N_ALIENI;i++){
    if(infoAli[i].morto==1){
      nAlieniMorti++;
    }
  }
  if(nAlieniMorti >= N_ALIENI ){
    return 1;
  } else {

    return 0;
  }
}


void stampaNavicella (InfoPipe info){
  int i,j;
  char sprite[3][3]={{'=','\\',' '},
                    {' ','@','>'},
                    {'=','/',' '}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}
void stampaNavicella1 (InfoPipe info){
  int i,j;
  char sprite[3][3]={{' ','\\',' '},
                    {' ','@','>'},
                    {' ','/',' '}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}
void stampaNavicella2 (InfoPipe info){
  int i,j;
  char sprite[3][3]={{' ','\\',' '},
                    {' ','o','>'},
                    {' ','/',' '}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaAlienolv1 (InfoPipe info){
  int i,j;
  char sprite[3][3]={{' ','/','='},
                    {'(','o',' '},
                    {' ','\\','='}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaAlienolv2 (InfoPipe info){
  int i,j;
  char sprite[3][3]={{' ','o',' '},
                    {'o',' ','o'},
                    {' ','o',' '}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaAlienolv2Half (InfoPipe info){
  int i,j;
  char sprite[3][3]={{' ','o',' '},
                    {' ',' ',' '},
                    {' ','o',' '}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void cancella3x3 (InfoPipe info){
  int i,j;
  char sprite[3][3]={{' ',' ',' '},
                    {' ',' ',' '},
                    {' ',' ',' '}};

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaBackground (char* background){

mvprintw(MAXY+2, 0, background);

}


char* generaBackgroundVuoto (){
  char *background = (char*) malloc(MAXX * sizeof(char));;

  int i;

  for (i=0;i<MAXX;i++){
    background[i]=' ';
  }

  return background;
}

char* generaBackground(){
  char *background = (char*) malloc(MAXX * sizeof(char));;

  char sprite[3][5]={{' ','-',' ',' ','-'},
                    {'=',' ',' ','-',' '},
                    {'/','\\','_','/','\\'}};
  int i;

  for(i=0;i<MAXX;i++){
    background[i]=sprite[2][i%5];
  }
  return background;

}

char* spostaBackground(char background[]){
  int i;
  char temp;
  char* t = &temp;

  t=background;

  for(i=1;i<MAXX;i++){
    background[i-1]=background[i];
  }

  background[MAXX-1]=*t;

  return background;
}
