
# makefile modbus bm280
# shep

BIN=../bin
SRC= .

all: $(BIN)/mb280  slave.o bme280.o

# compiler
CC=gcc
# debug
DEBUG= #-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall 
 
PTHREAD=-pthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

# linker
LD=gcc
LDFLAGS=$(PTHREAD)  -export-dynamic -l modbus  -lm


PPFLAGS=-nbad -nbap -bbo -bc -br -brs -c33 -cd33 -cdb -ce -ci4 -cli0\
-cp33 -di16 -fc1 -fca -hnl -i4 -ip4 -l75 -lp -npcs -nprs -psl\
-saf -sai -saw -sc -nsob -nss -ts8

pp:
	-@rm *.o	
	-@indent $(PPFLAGS) slave.c -o slave.c
	-@indent $(PPFLAGS) alarms.c -o alarms.c
	-@indent $(PPFLAGS) client.c -o client.c
	-@indent $(PPFLAGS) m.h -o m.h
	-@indent $(PPFLAGS) alarms.h -o alarms.h
	-@indent $(PPFLAGS) fdio.c -o fdio.c
	
 
$(BIN)/mb280: slave.o bme280.o fdio.o alarms.o
	$(LD) -o $(BIN)/mb280 slave.o fdio.o bme280.o alarms.o $(LDFLAGS) -lwiringPi
    
alarms.o: alarms.c m.h alarms.h
	$(CC) -c $(CCFLAGS) alarms.c -o alarms.o
      
slave.o: slave.c m.h alarms.h mbcom.h
	$(CC) -c $(CCFLAGS) slave.c -o slave.o
      
fdio.o: fdio.c m.h alarms.h mbcom.h
	$(CC) -c $(CCFLAGS) fdio.c -o fdio.o
     
client: client.c
	gcc client.c -o $(BIN)/client -pthread -Wall -l modbus -l m
   
bme280.o:  bme280.c bme280.h
	$(CC) -c $(CCFLAGS) bme280.c -o bme280.o
		
clean:
	rm -f *.o $(BIN)/mb280 $(BIN)/client
