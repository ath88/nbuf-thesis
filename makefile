CC=pdflatex
VIEWER=evince


LIBS=

all:
	$(CC) $(LIBS) thesis.tex
	$(CC) $(LIBS) thesis.tex
	$(VIEWER) thesis.pdf &