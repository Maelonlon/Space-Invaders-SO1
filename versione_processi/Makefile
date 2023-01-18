play: main.o controllo.o navicella.o alieno.o background.o
	gcc main.o controllo.o navicella.o alieno.o background.o -o play -lncurses

main.o: progetto.h main.c
	gcc -c main.c -lncurses

controllo.o: progetto.h controllo.c
	gcc -c controllo.c -lncurses

navicella.o: progetto.h navicella.c
	gcc -c navicella.c -lncurses

alieno.o: progetto.h alieno.c
	gcc -c alieno.c -lncurses

background.o: progetto.h background.c
		gcc -c background.c -lncurses

clean:
	rm -f	*.o play null
