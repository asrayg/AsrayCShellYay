CC = gcc
CFLAGS = -Wall -Wextra -g

all: shell

shell: main.o builtins.o utils.o job_control.o readline.o
	$(CC) $(CFLAGS) -o shell main.o builtins.o utils.o job_control.o readline.o -lreadline

main.o: main.c shell.h
	$(CC) $(CFLAGS) -c main.c

builtins.o: builtins.c builtins.h shell.h
	$(CC) $(CFLAGS) -c builtins.c

utils.o: utils.c utils.h shell.h
	$(CC) $(CFLAGS) -c utils.c

job_control.o: job_control.c job_control.h shell.h
	$(CC) $(CFLAGS) -c job_control.c

readline.o: readline.c readline.h shell.h
	$(CC) $(CFLAGS) -c readline.c

clean:
	rm -f *.o shell
