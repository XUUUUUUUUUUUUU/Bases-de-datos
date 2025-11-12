CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi

EXE= library
HEADERS=
%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<
