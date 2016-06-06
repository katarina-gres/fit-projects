

#XQR:xhavle03
#
# Soubor: expr.py
# Datum: 27. 3. 2014
# Autor: Vojtech Havlena, xhavle03
#
# Zpracovani vyrazu (analyza zhara nahoru) pro 2. projekt do predmetu IPP
#

from lex import *
from xmloperation import *

# Chybove konstanty
NONTERM = -1
DOLLAR = 42
VARIABLE = 41

# Precedencni tabulka pro vyhodnocovani operatoru.
PRECTABLE = { TAND:{ TAND:">", TOR:">", TNOT:"<", TLBRACE:"<", TRBRACE:">", VARIABLE: "<", DOLLAR:">"},
	TOR:{ TAND:"<", TOR:">", TNOT:"<", TLBRACE:"<", TRBRACE:">", VARIABLE: "<", DOLLAR:">"},
	TNOT:{ TAND:">", TOR:">", TNOT:"<", TLBRACE:"<", TRBRACE:">", VARIABLE: "<", DOLLAR:">"},
	TLBRACE:{ TAND:"<", TOR:"<", TNOT:"<", TLBRACE:"<", TRBRACE:"=", VARIABLE: "<", DOLLAR:""},
	TRBRACE:{ TAND:">", TOR:">", TNOT:"", TLBRACE:"", TRBRACE:">", VARIABLE: "", DOLLAR:">"},
	VARIABLE:{ TAND:">", TOR:">", TNOT:"", TLBRACE:"", TRBRACE:">", VARIABLE: "", DOLLAR:">"},
	DOLLAR:{ TAND:"<", TOR:"<", TNOT:"<", TLBRACE:"<", TRBRACE:"", VARIABLE: "<", DOLLAR:"$"}}

# Trida reprezentujici polozku na zasobniku.
class StackItem:
	def __init__(self):
		self.condSet = list()
		self.state = NONTERM
	def __init__(self, condSet, state):
		self.condSet = condSet
		self.state = state

#
# Funkce, ktera vrati neterminal nejblize vrcholu.
# @param lst Zasobnik
# @return 1. neterminal, resp -1
#
def Top(lst):
	for item in lst:
		if item.state != NONTERM:
			return item
	return -1

#
# Funkce, ktera zpracuje jednotlivou podminku.
# @param condSet Mnozina zatim vyhovujicich elementu.
# @param element Nazev elementu (1. operandu)
# @param attr Nazev pripadneho atributu.
# @param relation Relace porovnani
# @param literal Konstanta (2. operand)
# @param isInt Je literal celociselny?
# @return Novy seznam vyhovujicich elementu.
#
def ProcessCondition(condSet, element, attr, relation, literal, isInt):
	copy = list()
	
	if relation == TMORE:
		for item in condSet:
			elem = FindFrom(item, element, attr)
			if elem == None:
				return list()
			elemText = GetElementValue(elem, attr)
			if elemText == None:
				return -1
			elemText = ConvertType(elemText, isInt)
			if elemText == None:
				return list()
			if elemText > literal:
				copy.append(item)
				
	elif relation == TLESS:
		for item in condSet:
			elem = FindFrom(item, element, attr)
			if elem == None:
				return list()
			elemText = GetElementValue(elem, attr)
			if elemText == None:
				return -1
			elemText = ConvertType(elemText, isInt)
			if elemText == None:
				return list()
			if elemText < literal:
				copy.append(item)
				
	elif relation == TEQ:
		for item in condSet:
			elem = FindFrom(item, element, attr)
			if elem == None:
				return list()
			elemText = GetElementValue(elem, attr)
			if elemText == None:
				return -1
			elemText = ConvertType(elemText, isInt)
			if elemText == None:
				return list()
			if elemText == literal:
				copy.append(item)
				
	elif relation == TCONTAINS:
		for item in condSet:
			elem = FindFrom(item, element, attr)
			if elem == None:
				return list()
			if isInt: # CONTAINS musi byt s retezcem
				return None
			
			elemText = GetElementValue(elem, attr)
			if elemText == None:
				return -1
			elemText = ConvertType(elemText, isInt)
			if elemText == None:
				return list()
			if literal in elemText:
				copy.append(item)
				
	return copy

#
# Funkce, ktera zpracuje promennou (elementarni podminku).
# @param lex Lexikalni analyzator.
# @param condSet Mnozina zatim vyhovujicich elementu.
# @return Novy seznam vyhovujicich elementu.
#
def ProcessVariable(lex, condSet):
	attr = None
	element = None
	if lex.token == TELEMENT:
		element = lex.value
		lex.GetToken()
	
	if lex.token == TDOT:
		lex.GetToken()
		if lex.token != TELEMENT:
			return None
		attr = lex.value
		lex.GetToken()

	if lex.token != TLESS and lex.token != TMORE and lex.token != TEQ and lex.token != TCONTAINS:
		return None
	relation = lex.token
	lex.GetToken()
	if lex.token != TSTRING and lex.token != TNUMBER:
		return None
	return ProcessCondition(condSet, element, attr, relation, lex.value, (lex.token == TNUMBER))

#
# Funkce, ktera vyhodnoti podminku
# @param lex Lexikalni analyzator.
# @param condSet Mnozina zatim vyhovujicich elementu.
# @return Seznam vyhovujicich elementu.
#
def EvalueateCondition(lex, condSet):
	lst = list()
	originalList = condSet
	
	lst.insert(0, StackItem(condSet, DOLLAR))
	lex.GetToken()
	while True:
		a = Top(lst)
		if a == -1:
			return None
		b = lex.token
		if b == TEOF:
			b = DOLLAR
		
		cp = list()
		if lex.token == TELEMENT or lex.token == TDOT:
			cp = ProcessVariable(lex, condSet)
			if cp == None:
				return None
			elif cp == -1:
				return 4
			b = VARIABLE
		
		try:
			index = PRECTABLE[a.state][b]
		except KeyError:
			return None
			
		if index == "=" or index == "<":
			lst.insert(0, StackItem(cp, b))
			lex.GetToken()
		elif index == ">":
			if len(lst) == 0:
				return -1
			if lst[0].state == VARIABLE:
				lst[0].state = NONTERM
			elif lst[0].state == TRBRACE and len(lst) >= 3:
				if lst[1].state != NONTERM:
					return -1
				if lst[2].state != TLBRACE:
					return -1
				top = lst[1]
				lst = lst[3:]
				lst.insert(0, StackItem(top.condSet,NONTERM))
			elif lst[0].state == NONTERM and len(lst) >= 2:
				if lst[1].state == TNOT:
					cp = lst[0].condSet
					lst = lst[1:]
					lst[0].condSet = GetListDifference(originalList, cp)  #set([1,2,3,4,5,6,7,8,9,10]) - cp
					lst[0].state = NONTERM
					
				elif len(lst) >= 3:
					if lst[1].state == TAND and lst[2].state == NONTERM:
						tst = GetListIntersection(lst[0].condSet, lst[2].condSet)
						lst = lst[3:]
						lst.insert(0, StackItem(tst,NONTERM))
					elif lst[1].state == TOR and lst[2].state == NONTERM:
						tst = GetListUnion(lst[0].condSet, lst[2].condSet)
						lst = lst[3:]
						lst.insert(0, StackItem(tst,NONTERM))
					else:
						return -1
				else:
					return -1
			else:
				return -1
		elif index == "$":
			return lst[0].condSet
		else:
			return -1
		if a == DOLLAR and Top(lst) == DOLLAR:
			break
	return lst[0].condSet

#
# Funkce, ktera vzajemne prevede datove typy.
# @param op Operator, ktery se ma prevest.
# @param isInt Jedna se o cislo.
# @return Prevedena hodnota.
#	
def ConvertType(op, isInt):
	if isInt:
		try:
			res = float(op)
		except TypeError:
			return None
		except ValueError:
			return None
	else:
		try:
			res = str(op)
		except TypeError:
			return None
		except ValueError:
			return None
	return res
