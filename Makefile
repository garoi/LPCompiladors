CC = g++
CFLAGS = -I/usr/include/pccts/
compres: compres.c
	$(CC) -o compres compres.c scan.c err.c $(CFLAGS)
	rm -f *.o scan.c err.c parser.dlg tokens.h mode.h
compres.c: compres.g
	antlr -gt compres.g
	dlg -ci parser.dlg scan.c
clean:
	rm -f *.o compres compres.c scan.c err.c parser.dlg tokens.h mode.h