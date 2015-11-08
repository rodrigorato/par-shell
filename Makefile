# PAR-SHELL MAKEFILE USAGE:
# make par-shell : compila o projeto fazendo uso de ficheiros objecto
# make clean : elimina ficheiros objecto e o executavel da par-shell

par-shell: par-shell.o list.o error_helper.o  commandlinereader.o fibonacci
	gcc -pthread -o par-shell par-shell.o error_helper.o commandlinereader.o list.o

par-shell.o: par-shell.c list.o
	gcc -Wall -g -c par-shell.c

list.o: list.c list.h
	gcc -Wall -g -c list.c 

error_helper.o: error_helper.c error_helper.h
	gcc -Wall -g -c error_helper.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

fibonacci: fibonacci.o
	gcc -o fibonacci fibonacci.o

fibonacci.o: fibonacci.c
	gcc -Wall -g -c fibonacci.c

cleanlog:
	rm -f *.o par-shell fibonacci log.txt

clean: 
	rm -f *.o par-shell fibonacci
