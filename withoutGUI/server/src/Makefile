server : server.o handler.o
	gcc server.o handler.o -std=gnu99 -o server -lpthread -Wall

handler.o : handler.h handler.c utli.h 
	gcc -c handler.c -std=gnu99 -Wall

server.o : server.c handler.h
	gcc -c server.c -std=gnu99 -Wall

clean :
	rm server.o handler.o