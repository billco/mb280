
# makefile modbus bm280
# shep

BIN=../bin

all: $(BIN)/mb280  slave.o bme280.o
#OUTPUT_OPTION = -MMD -MP -o $@
#-include ${DEPS}

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


 
$(BIN)/mb280: slave.o bme280.o fdio.o
	$(LD) -o $(BIN)/mb280 slave.o fdio.o bme280.o $(LDFLAGS) -lwiringPi
    
slave.o: slave.c m.h
	$(CC) -c $(CCFLAGS) slave.c -o slave.o
     
fdio.o: fdio.c m.h
	$(CC) -c $(CCFLAGS) fdio.c -o fdio.o
     
client: client.c client.o mc.h
	$(CC)  $(CCFLAGS) client.c -o $(BIN)/client -l modbus -l m
   
bme280.o:  bme280.c bme280.h
	$(CC) -c $(CCFLAGS) bme280.c -o bme280.o
help:
	-@echo "mb280 : make executable 'make'"
	-@echo "client: make executable 'make client'"
	-@echo "clean : delete all executables and object files 'make clean'"
	
clean:
	rm -f *.o $(BIN)/mb280 $(BIN)/client
