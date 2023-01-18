play: main.o controllo.o navicella.o alieno.o background.o
	gcc main.o controllo.o navicella.o alieno.o background.o -o play -lncurses -lpthread

main.o: progetto.h main.c
	gcc -c main.c -lncurses -lpthread

controllo.o: progetto.h controllo.c
	gcc -c controllo.c -lncurses -lpthread

navicella.o: progetto.h navicella.c
	gcc -c navicella.c -lncurses -lpthread

alieno.o: progetto.h alieno.c
	gcc -c alieno.c -lncurses -lpthread

background.o: progetto.h background.c
	gcc -c background.c -lncurses -lpthread

clean:
	rm -f	*.o play null
