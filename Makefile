# PAR-SHELL MAKEFILE USAGE:
# make par-shell : compila o projeto fazendo uso de ficheiros objecto mas nao limpa o log
# make clean : elimina ficheiros objecto, o executavel da par-shell e o log.txt
# make cleanlog : elimina o ficheiro log.txt

par-shell: par-shell.o list.o helper.o  commandlinereader.o fibonacci par-shell-terminal.o 
	gcc -pthread -o par-shell par-shell.o helper.o commandlinereader.o list.o 
	gcc -o par-shell-terminal par-shell-terminal.o helper.o commandlinereader.o 
	
par-shell-terminal.o: par-shell-terminal.c
	gcc -Wall -g -c par-shell-terminal.c

par-shell.o: par-shell.c list.o
	gcc -Wall -g -c par-shell.c

list.o: list.c list.h
	gcc -Wall -g -c list.c 

helper.o: helper.c helper.h
	gcc -Wall -g -c helper.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

fibonacci: fibonacci.o
	gcc -o fibonacci fibonacci.o

fibonacci.o: fibonacci.c
	gcc -Wall -g -c fibonacci.c

cleantxts:
	rm log.txt par-shell-out-*

cleanpipes:
	rm par-shell-in

clean: 
	rm -f *.o par-shell fibonacci log.txt par-shell-out-* par-shell-terminal par-shell-in /tmp/par-shell-in /tmp/par-shell-stats.txt
