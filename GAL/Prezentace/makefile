#
# 3. Projekt do predmetu ITY
# Autor: Vojtech Havlena <xhavle03@stud.fit.vutbr.cz>
# Datum: 2. 4. 2013
#

PROJEKT=prezentace
proj: $(PROJEKT).tex
	latex $(PROJEKT).tex $(PROJEKT).dvi
	latex $(PROJEKT).tex $(PROJEKT).dvi
	latex $(PROJEKT).tex $(PROJEKT).dvi
	dvips $(PROJEKT).dvi 
	ps2pdf -sPAPERSIZE=a4 $(PROJEKT).ps $(PROJEKT).pdf
