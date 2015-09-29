# PAR-SHELL MAKEFILE USAGE:
# make par-shell : compila o projeto fazendo uso de ficheiros objecto
# make clean : elimina ficheiros objecto e o executavel da par-shell

par-shell: par-shell.o list.o commandlinereader.o
	gcc -o par-shell par-shell.o commandlinereader.o list.o

par-shell.o: par-shell.c
	gcc -Wall -g -c par-shell.c 

list.o: list.c list.h
	gcc -Wall -g -c list.c 

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

clean: 
	rm -f *.o par-shell
