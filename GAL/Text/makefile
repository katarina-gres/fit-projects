#
# Projekt do predmetu Grafove algoritmy -- GAL (Generovani bludist)
# Autor: Vojtech Havlena <xhavle03@stud.fit.vutbr.cz>
# Datum: 10. 11. 2015
#

NAME=xhavle03

all: $(NAME).tex title.tex
	pdflatex $(NAME).tex
	bibtex $(NAME).aux
	pdflatex $(NAME).tex
	pdflatex $(NAME).tex

clean:
	-rm $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out
