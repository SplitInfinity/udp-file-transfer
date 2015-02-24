all: server deliver
	 rm *.o ./server/*.o
	 
server: server.o utils.o
	gcc -g ./server/server.o utils.o -o ./server/server -lm

deliver: deliver.o utils.o
	gcc -g deliver.o utils.o -o deliver  -lm

deliver.o: deliver.c 
	gcc -g deliver.c -c

server.o: ./server/server.c
	gcc -g ./server/server.c -c -o ./server/server.o

utils.o: utils.c utils.h
	gcc -g utils.c -c 

clean:
	rm deliver.o ./server/server.o utils.o deliver ./server/server *~ ./server/*~
