CC = gcc
CFLAGS = -Wall -g
OBJ = calc.o calc_logic.o
CLEAN = *.o *.exe
all: calc

calc: $(OBJ)
	$(CC) $(CFLAGS) -o calc $(OBJ)

calc.o: calc.c calc_logic.h
	$(CC) $(CFLAGS) -c calc.c

calc_logic.o: calc_logic.c calc_logic.h
	$(CC) $(CFLAGS) -c calc_logic.c

clean:
	rm -f $(CLEAN)
direct: calc.c calc_logic.c
	gcc calc.c calc_logic.c -o calc 
