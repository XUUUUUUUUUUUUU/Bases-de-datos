CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi

EXE= library
OBJ= $(EXE).o

.PHONY: clean run valgrind

%.o: %.c
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE): $(OBJ)
	@echo Linking $(EXE)...
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) 

clean:
	@rm -f *.o core $(EXE)

run:
	./$(EXE)

valgrind:
	valgrind --leak-check=full -s --log-file=valgrind.log ./$(EXE)