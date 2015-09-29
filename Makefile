# PAR-SHELL MAKEFILE USAGE:
# make par-shell : compila o projeto fazendo uso de ficheiros objecto
# make clean : elimina ficheiros objecto e o executavel da par-shell

par-shell: par-shell.o
	gcc -o par-shell par-shell.o

par-shell.o:
	gcc -Wall -g -c par-shell.c 

clean: 
	rm -f *.o par-shell
