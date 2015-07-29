COMPILER=gcc
CFLAGS=-Wall
all:echotcp
echotcp: server client
server:  
	$(COMPILER) -o server ServerMain.c

client: 
	$(COMPILER) -o client ClientMain.c


clean:
	rm *.o server client



