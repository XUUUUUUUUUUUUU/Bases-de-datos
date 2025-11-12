CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc -lcurses -lpanel -lmenu -lform

export PGDATABASE := flight
export PGUSER := alumnodb
export PGPASSWORD := alumnodb
export PGCLIENTENCODING := LATIN9
export PGHOST := localhost

DBNAME := $(PGDATABASE)
PSQL := psql
CREATEDB := createdb
DROPDB := dropdb --if-exists
PG_DUMP := pg_dump
PG_RESTORE := pg_restore

# recompile if these headers changes
HEADERS = ../odbc.h bpass.h lmenu.h search.h windows.h loop.h

EXE = menu
OBJ = $(EXE).o bpass.o loop.o search.o windows.o ../odbc.o

.PHONY: all clean run valgrind test01 createdb dropdb dump restore psql shell

all: $(EXE)

test01: dropdb createdb restore shell

createdb:
	@echo "Creando BBDD $(DBNAME)"
	@$(CREATEDB) $(DBNAME)

dropdb:
	@echo "Eliminando BBDD $(DBNAME)"
	@$(DROPDB) $(DBNAME)
	@rm -f *.log

dump:
	@echo "creando dumpfile ../$(DBNAME).sql"
	@$(PG_DUMP) $(DBNAME) > ../$(DBNAME).sql

restore:
	@echo "restore data base $(DBNAME)"
	@cat ../$(DBNAME).sql | $(PSQL) $(DBNAME)

psql: shell

shell:
	@echo "create psql shell (conectando a $(DBNAME))"
	@$(PSQL) -d $(DBNAME)

# compile all files ending in *.c
%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

# link binary
$(EXE): $(OBJ)
	@echo Linking $(EXE)...
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LDLIBS)

clean:
	@rm -f *.o ../odbc.o core $(EXE)

run:
	./$(EXE)

valgrind:
	valgrind --leak-check=full --log-file=valgrind.log ./$(EXE)
