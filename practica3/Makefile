CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi

EXE= library
OBJ= $(EXE).o del.o add.o find.o 
FILENAME=test

.PHONY: clean run valgrind best_fit first_fit worst_fit

%.o: %.c
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE): $(OBJ) types.h
	@echo Linking $(EXE)...
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) 

clean:
	@rm -f *.o core $(EXE) $(FILENAME).db $(FILENAME).ind $(FILENAME).lst

run: $(EXE)
	./$(EXE) best_fit $(FILENAME)

best_fit: $(EXE)
	./$(EXE) best_fit $(FILENAME)

first_fit: $(EXE)
	./$(EXE) first_fit $(FILENAME)

worst_fit: $(EXE)
	./$(EXE) worst_fit $(FILENAME)

valgrind:
	valgrind --leak-check=full -s ./$(EXE) first_fit test