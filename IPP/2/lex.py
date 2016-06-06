

#XQR:xhavle03
#
# Soubor: lex.py
# Datum: 27. 3. 2014
# Autor: Vojtech Havlena, xhavle03
#
# Lexikalni analyzator pro 2. projekt do predmetu IPP
#

import sys

TSELECT = 0
TLIMIT = 1
TFROM = 2
TROOT = 3
TWHERE = 4
TORDER = 5
TBY = 6
TNOT = 7
TAND = 8
TOR = 9
TCONTAINS = 10

TSTRING = 15
TNUMBER = 16
TELEMENT = 17
TDOT = 18
TEOF = 19
TLBRACE = 20
TRBRACE = 21
TLESS = 22
TMORE = 23
TEQ = 24
TERROR = 30

#
# Trida pro lexikalni analyzator
#
class Lexical:
	
	# Klicova slova
	keywords = ["SELECT", "LIMIT", "FROM", "ROOT", 
		"WHERE", "ORDER", "BY", "NOT", "AND", "OR", "CONTAINS"]
	token = TERROR # Aktualne nacteny token
	value = "" #Hodnota lexemu
	history = "" #Historie nacitaneho textu
	
	#
	# Konstruktor
	# @param queryString Retezec, ktery se ma zpracovat na vstupu
	# @param queryFile Soubor, ze ktereho se ma vstup nacist
	#
	def __init__(self, queryString = None, queryFile = None):
		if queryFile != None:
			f = open(queryFile, "r") #muze vyvola vyjimku!!!
			self.strInput = f.read()
			f.close()
		elif queryString != None:
			self.strInput = queryString
		else:
			self.strInput = sys.stdin.read()
	
	#
	# Metoda, ktera nacte dalsi retezec ze vstupu.
	# @return Dalsi znak ze vstupu.
	#
	def GetChar(self):
		if len(self.history) > 0:
			tmp = self.history[0]
			self.history = self.history[1:]
			return tmp
		elif len(self.strInput) > 0:
			tmp = self.strInput[0]
			self.strInput = self.strInput[1:]
			return tmp
		else:
			return -1
	
	#
	# Metoda, ktera vrati znak zpet na vstup.
	# @param char Znak, ktery se ma vratit na vstup.
	#
	def UngetChar(self, char):
		self.history = char + self.history
		
	def RecognizeKeyWords(self, keyword):
		if keyword in self.keywords:
			self.token = self.keywords.index(keyword)
		else:
			self.token = TELEMENT
	
	#
	# Metoda, ktera zpracuje dalsi lexem ze vstupu a pokusi se ulozit
	# informace o tokenu.
	#	
	def GetToken(self):
		state = 0
		keyIndex = -1
		self.token = TERROR
		self.value = ""

		while 1:
			char = self.GetChar()
			if char == -1:
				break
			
			if state == 0:
				if char.isspace():
					continue
				if char.isdigit() or char == '-' or char == '+':
					state = 1
					self.token = TNUMBER
				elif char == ".":
					self.token = TDOT
					return
				elif char == "\"":
					self.token = TSTRING
					state = 3
				elif char == "(":
					self.token = TLBRACE
					return
				elif char == ")":
					self.token = TRBRACE
					return
				elif char == "<":
					self.token = TLESS
					return
				elif char == ">":
					self.token = TMORE
					return
				elif char == "=":
					self.token = TEQ
					return
				else:
					state = 2
			elif state == 1:
				if char.isspace() or char == ")":
					self.token = TNUMBER;
					self.value = int(self.value)
					self.UngetChar(char)
					return
				if not char.isdigit():
					#state = 2
					#if char.isspace() or char == ".":
					#	self.UngetChar(char)
					#	self.RecognizeKeyWords(self.value)
					#	return
					self.token = TERROR;
					self.UngetChar(char)
					return
			elif state == 2:
				if char.isspace() or char == ".":
					self.UngetChar(char)
					self.RecognizeKeyWords(self.value)
					return
			elif state == 3:
				if char == "\"":
					#self.value += char #uvozovky uz nechceme
					self.value = self.value[1:]
					return
			else:
				self.token = TERROR
				return
			self.value += char
		if len(self.value) == 0:
			self.token = TEOF
		elif self.token == TERROR:
			self.RecognizeKeyWords(self.value)
		elif self.token == TSTRING:
			self.token = TERROR
		elif self.token == TNUMBER:
			self.value = int(self.value)
			

