CC = gcc
CFLAGS = -Wall -g
CLEAN = sum_serial sum_multi_thread

serial: sum_serial.c
	$(CC) $(CFLAGS)  sum_serial.c -o sum_serial
multi: sum_multi_thread.c
	$(CC)  sum_multi_thread.c -o sum_multi_thread

clean: 
	rm -f $(CLEAN)

