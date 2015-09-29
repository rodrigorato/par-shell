# PAR-SHELL MAKEFILE USAGE:
# make par-shell : compila o projeto fazendo uso de ficheiros objecto
# make clean : elimina ficheiros objecto e o executavel da par-shell

par-shell: par-shell.o
	gcc -o par-shell par-shell.o

par-shell.o:  list.o commandlinereader.o
	gcc -Wall -g -c par-shell.c 

list.o: 
	cd lists/ && gcc -Wall -g -c list.c && mv list.o ../list.o

commandlinereader.o:
	cd cmdlinerdr/ && gcc -Wall -g -c commandlinereader.c && mv commandlinereader.o ../commandlinereader.o

clean: 
	rm -f *.o par-shell
