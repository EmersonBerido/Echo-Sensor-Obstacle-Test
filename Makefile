CC = gcc
CFLAGS = -Wall -g
LIBS = -lpigpio -lrt -pthread

ROOTNAME = assignment2.c
ADDOBJ = line_sensor.c

EXE = assignment2

all: $(EXE)

$(EXE): $(ROOTNAME)
	$(CC) $(CFLAGS) $(ROOTNAME) $(ADDOBJ) -o $(EXE) $(LIBS)

clean:
	rm -f $(EXE)

run: $(EXE)
	sudo ./$(EXE)