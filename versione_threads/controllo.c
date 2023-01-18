#include "progetto.h"

void Controllo(int pipe, int pidAlieno[]){/*qui gestiremo le collisioni e la stampa degli elementi*/

  InfoPipe info, infoNav;
  InfoPipe* infoAli, *infoMiss, *infoBomba;

  int i,j,n=0, vite=VITE;
  int vinto=1;
  int endgame=0;
  int* viteAlieno;

  char background[MAXX], backgroundVuoto[MAXX];

  viteAlieno = (int*)malloc (sizeof(int)*N_ALIENI); //vite dell'alieno
  infoAli = (InfoPipe*)malloc (sizeof(InfoPipe)*N_ALIENI); //struttura contenente le informazioni dell'alieno
  infoMiss = (InfoPipe*)malloc (sizeof(InfoPipe)*60); //struttura contenente le informazioni del missile
  infoBomba = (InfoPipe*)malloc (sizeof(InfoPipe)*50); //struttura contenente le informazioni della bomba nemica
	

    //generazione dei background
   strcpy(background, generaBackground());
   strcpy(backgroundVuoto, generaBackgroundVuoto());


  while(!endgame){

    info = bufferOut(); //la struttura che esce dal buffer viene trascritta in info

    //COLLISIONI

     if(info.type=='a'){
    //se l'alieno e la navicella entrano nello stesso range di x (linea verticale), allora end game
       if(info.x<=infoNav.x+2){
    
    
         pthread_cancel(info.tid); //cancello il thread delle info dell'alieno
         info.morto=1;            //siamo morti
    
         pthread_cancel(infoNav.tid); //cancello il tread della navicella
         infoNav.morto=1; //vuol dire che siamo morti
	vinto=0; //vuol dire che abbiamo perso
         cancella3x3(infoNav); //cancello sprite navicella
       }
     }
    

  if(info.type=='m'){
//se il missile becca l'alieno allora :
      for(i=0;i<N_ALIENI;i++){
         if(infoAli[i].x+1==info.x && infoAli[i].y==info.y ||
           infoAli[i].x==info.x && infoAli[i].y+1==info.y ||
           infoAli[i].x+1==info.x && infoAli[i].y+2==info.y){
      
              viteAlieno[i]++;
      /*le vite sono contate con l'addizione, in modo da confrontarle con VITE_ALIENO. 
      Se il numero di vite è superiore al range consentito allora succederà che l'alieno muore
      */
           if(viteAlieno[i]>=VITE_ALIENO){
             cancella3x3(infoAli[i]);//cancello sprite alieno
             mvaddch(info.y, info.x, ' ');//sovrascrivo gli spazi colorati dall'alieno con spazi vuoti
      
             infoAli[i].morto=1; //vuol dire che l'alieno è morto
		infoAli[i].x=-1; //l'alieno avanza di una posizione
		infoAli[i].y=-1; //l'alieno avanza di una posizione
             info.morto=1;//il missile muore
             pthread_cancel(infoAli[i].tid);//elimino il thread dell'alieno
             pthread_cancel(info.tid);//elimino il thread delle info del missile
      
           }
         }
       }

	      if(info.x >= MAXX-1){ //se supero la distanza massima dalla navicella col missile, il missile muore
		mvaddch(info.y, info.x, ' ');

		info.morto = 1;
		pthread_cancel(info.tid); //cancello il thread delle info del missile
	      }

  	}


    if(info.type=='b'){

      //se la bomba colpisce la navicella allora:
      if(infoNav.x+1==info.x && infoNav.y==info.y ||
         infoNav.x+2==info.x && infoNav.y+1==info.y ||
         infoNav.x+1==info.x && infoNav.y+2==info.y){

        vite--; //perdiamo una vita
        //mvaddch(info.y, info.x, ' ');
        pthread_cancel(info.tid); //cancelliamo il thread della bomba

            if(vite<=0){
              //se non abbiamo piu vite allora:
              infoNav.morto=1;//muoriamo
              vinto=0;//perdiamo
              cancella3x3(infoNav);//cancello sprite navicella
              pthread_cancel(infoNav.tid);//cancello il thread della navicella
            }
        info.morto=1; //la bomba muore
      }

      if(info.x <= 1){ //se il missile supera una certa distanza allora va killato
	    mvaddch(info.y, info.x, ' '); //sovrascrivo il disegno del missile con uno spazio vuoto
      info.morto = 1;//il missile muore
      pthread_cancel(info.tid);//muore il thread del missile
      }

    }





    //STAMPA

    if(info.type=='n'){
      cancella3x3(infoNav);//cancello lo sprite della navicella
      infoNav=info;//inserisco le informazioni di info nella navicella

      if(!infoNav.morto){ //se la navicella è viva:


          //a seconda delle vite stampo uno sprite differente
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

    if(info.type=='a'){

        cancella3x3(infoAli[info.id]);//cancello sprite alieno
        infoAli[info.id]=info;//sovrascrivo le informazioni dell'alieno con l'info dell'alieno corrente
        if(!infoAli[info.id].morto){ //se l'alieno è vivo:
          //stampo uno sprite dell'alieno a seconda di quante vite ha
          if(viteAlieno[info.id]==0){
            stampaAlienolv1(info);
           } else if(viteAlieno[info.id]==1){
            stampaAlienolv2(info);
          } else {
            stampaAlienolv2Half(info);

        }
      }
    }

    if(info.type=='m'){
        
        mvaddch(infoMiss[info.id].y, infoMiss[info.id].x, ' ');//cancello lo sprite del missile

        infoMiss[info.id]=info; //sovrascrivo il missile con il missile corrente passato da info
        if(!infoMiss[info.id].morto){ //se il missile c'è allora lo stampo

        mvaddch(info.y, info.x, '-');
      }
    }


    if(info.type=='b'){

        mvaddch(infoBomba[info.id].y, infoBomba[info.id].x, ' ');//metto uno spazio vuoto dove stava la bomba
        infoBomba[info.id]=info; //sovrascrivo la bomba precedente con la bomba corrente
        if(!infoBomba[info.id].morto){ //se la bomba esiste allora la stampo 

        mvaddch(info.y, info.x, 'o');
      }
    }
  
     if(info.type=='B'){
      //stampo il background 
       strcpy(background, spostaBackground(background));
       stampaBackground(backgroundVuoto);
       stampaBackground(background);
  
     }
  
  
     endgame = endgameCheck(infoAli, infoNav); //serve a cancellare tutti i thread per la fine del gioco
  
  
  //   refresh();
  //
  // }
  // for(i=0;i<69;i++){
  //   if(infoMiss[i].pid>0){
  //   kill(infoMiss[i].pid, 1);
  // }
  // }
  // for(i=0;i<50;i++){
  //   if(infoBomba[i].pid>0){
  //   kill(infoBomba[i].pid, 1);
  refresh();
  //   }
  }
  //free serve a liberare la memoria del parametro passato
	free(viteAlieno);
	free(infoAli);
    free(infoMiss);
    free(infoBomba);

clear(); //pulizia della memoria

    if (vinto) //se abbiamo vinto
        mvprintw(MAXY / 2, MAXX / 2, "Player has won"); //abbiamo vinto
    else
        mvprintw(MAXY / 2, MAXX / 2, "Player has lost"); //abbiamo perso
refresh();
    getch();

}

int endgameCheck (InfoPipe* infoAli, InfoPipe infoNav){
  int nAlieniMorti=0, i;

  if(infoNav.morto==1){ //se la navicella è morta:
	for(i=0;i<N_ALIENI;i++){
		pthread_cancel(infoAli[i].tid);//cancello tutti i thread appartenenti agli alieni
	}
    return 1;
  }
  for(i=0;i<N_ALIENI;i++){
    if(infoAli[i].morto==1){//se gli alieni sono morti
      nAlieniMorti++; //li conto 
    }
  }
  if(nAlieniMorti >= N_ALIENI ){ //se gli alieni morti superano o uguagliano il numero degli alieni creati
	pthread_cancel(infoNav.tid);//allora cancello il thread della navicella
    return 1;
  } else {

    return 0;
  }
}


void stampaNavicella (InfoPipe info){ //stampa della navicella
  int i,j;
  

  char sprite[3][3]={{'=','\\',' '}, 
                    {' ','@','>'},
                    {'=','/',' '}};
  //stampa dello sprite 3x3
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaNavicella1 (InfoPipe info){//stampa della navicella
  int i,j;
  char sprite[3][3]={{' ','\\',' '},
                    {' ','@','>'},
                    {' ','/',' '}};
//stampa dello sprite 3x3
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}
void stampaNavicella2 (InfoPipe info){//stampa della navicella
  int i,j;
  char sprite[3][3]={{' ','\\',' '},
                    {' ','o','>'},
                    {' ','/',' '}};
//stampa dello sprite 3x3
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}




void stampaAlienolv1 (InfoPipe info){ //stampa dell'alieno
  int i,j;
  char sprite[3][3]={{' ','/','='},
                    {'(','o',' '},
                    {' ','\\','='}};
//stampa dello sprite 3x3
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaAlienolv2 (InfoPipe info){//stampa dell'alieno
  int i,j;
  char sprite[3][3]={{' ','o',' '},
                    {'o',' ','o'},
                    {' ','o',' '}};
//stampa dello sprite 3x3
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaAlienolv2Half (InfoPipe info){//stampa dell'alieno
  int i,j;
  char sprite[3][3]={{' ','o',' '},
                    {' ',' ',' '},
                    {' ','o',' '}};
//stampa dello sprite 3x3
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void cancella3x3 (InfoPipe info){ //cancella sprite 3x3
  int i,j;
  char sprite[3][3]={{' ',' ',' '},
                    {' ',' ',' '},
                    {' ',' ',' '}};
//stampa dello sprite 3x3 vuoto
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
    mvaddch(info.y+i, info.x+j, sprite[i][j]);
    }
  }
}

void stampaBackground (char* background){ //stampa del background
  mvprintw(MAXY+2, 0, background);
}


char* generaBackgroundVuoto (){ //creo lo spazio per generare un background
  char *background = (char*) malloc(MAXX * sizeof(char));;

  int i;

  for (i=0;i<MAXX;i++){
    background[i]=' ';
  }

  return background;
}

char* generaBackground(){ //genero il backgroundo
  char *background = (char*) malloc(MAXX * sizeof(char));;
//faccio scorrere questo sprite all'infinito
  char sprite[3][5]={{' ','-',' ',' ','-'},
                    {'=',' ',' ','-',' '},
                    {'/','\\','_','/','\\'}};
  int i;

  for(i=0;i<MAXX;i++){
    background[i]=sprite[2][i%5];
  }
  return background;

}
//funzione che permette al background di muoversi 
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
