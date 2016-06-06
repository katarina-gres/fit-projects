

#XQR:xhavle03
#
# Soubor: xqr.py
# Datum: 27. 3. 2014
# Autor: Vojtech Havlena, xhavle03
#
# Implementace ulohy do predmetu IPP XQR, XML Query.
#

import sys
import getopt
import xml.etree.ElementTree as ET
from lex import *
from syntax import *

# Chybove kody.
EFORMAT = 1
EFILEEXIST = 2
EFILEWRITE = 3
EFILEFORMAT = 4
ESYNTAX = 80

# Chybove hlasky pro jednotlive kody.
EMESSAGE = {EFORMAT : "Spatny format vstupnich parametru.\n",
	EFILEEXIST : "Vstupni soubor neexistuje nebo se jej neporadilo otevrit.\n",
	EFILEWRITE : "Nepodarilo se otevrit soubor pro zapis.\n",
	EFILEFORMAT : "Spatny format vstupniho souboru.\n",
	ESYNTAX : "Syntakticka nebo semanticka chyba.\n"}

def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], "n", ["help", "input=", "output=", "query=", "qf=", "root="])
	except getopt.GetoptError as err:
		PrintError(EFORMAT)
		sys.exit(EFORMAT)
    
	inputFile = None
	outputFile = None
	queryFile = None
	query = None
	root = None
	generateHeader = True

	for o, a in opts:
		if o == "--input" and StartArgvWith("--input=") and inputFile == None:
			inputFile = a
		elif o == "--output" and StartArgvWith("--output=") and outputFile == None:
			outputFile = a
		elif o == "--query" and StartArgvWith("--query=") and query == None:
			query = a
		elif o == "--qf" and StartArgvWith("--qf=") and queryFile == None:
			queryFile = a
		elif o == "--root" and StartArgvWith("--root=") and root == None:
			root = a
		elif o == "-n" and StartArgvWith("-n") and generateHeader:
			generateHeader = False
		elif o == "--help" and StartArgvWith("--help") and len(sys.argv) == 2:
			PrintHelp()
			sys.exit(0)
		else:
			PrintError(EFORMAT)
			sys.exit(EFORMAT)
	
	if query != None and queryFile != None:
		PrintError(EFORMAT)
		sys.exit(EFORMAT)
		
	
	try:
		if inputFile == None:
			tree = ET.ElementTree(ET.fromstring(sys.stdin.read()))
		else:
			tree = ET.parse(inputFile)
	except IOError:
		PrintError(EFILEEXIST)
		sys.exit(EFILEEXIST)
	except  ET.ParseError:
		PrintError(EFILEFORMAT)
		sys.exit(EFILEFORMAT)
	
	try:	
		lex = Lexical(query, queryFile)
	except IOError:
		PrintError(EFILEEXIST)
		sys.exit(ESYNTAX)
		
	res = Query(lex, tree)
	if res[0] != EOK:
		PrintError(res[0])
		sys.exit(res[0])
	
	# Vygenerujeme vystup
	if generateHeader:
		resString = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	else:
		resString = ""
	if root != None:
		newRoot = ET.Element(root)
		for item in res[1]:
			newRoot.append(item)
		resString += ET.tostring(newRoot).decode("utf-8")
	else:
		for item in res[1]:
			resString += ET.tostring(item).decode("utf-8")
	
	if resString != "":
		resString += "\n"

	# Vypiseme do souboru
	if outputFile != None:
		try:
			fOut = open(outputFile, "w")
			fOut.write(resString)
		except IOError:
			PrintError(EFILEEXIST)
			sys.exit(EFILEEXIST)
	else:
		sys.stdout.write(resString)
		sys.stdout.flush()
	

#
# Funkce, ktera vytiskne chybu na stderr.
# @param code Chybovy kod.
#
def PrintError(code):
	sys.stderr.write(EMESSAGE[code])

#
# Funkce, ktera kontroluje, zda argument prikazove radky 
# zacina zadanym textem.
#
def StartArgvWith(string):
	for item in sys.argv:
		if item.startswith(string):
			return True
	return False

#
# Funkce, ktera vytiskne napovedu.
#
def PrintHelp():
	print("XQR: XML Query, Vojtech Havlena, xhavle03")
	print("--help Napoveda k programu.")
	print("--input=filename Zadany vstupni soubor ve formatu XML.")
	print("--output=filenam Zadany vstupni soubor s obsahem podle dotazu.")
	print("--query='dotaz' Zadany dotaz.")
	print("--qf=filename Dotaz v externim souboru.")
	print("-n Negenerovat hlavicku XML na vystup.")
	print("--root=element Jmeno elementu obalujiciho vysledky.")

if __name__ == "__main__":
	main()

