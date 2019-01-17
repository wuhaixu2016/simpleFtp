client : client.o transMode.o
	gcc client.o transMode.o -std=gnu99 -o client -Wall
transMode.o : transMode.h transMode.c utli.h
	gcc -c transMode.c -std=gnu99 -Wall

client.o : client.c transMode.h
	gcc -c client.c -std=gnu99 -Wall

clean :
	rm client.o transMode.o