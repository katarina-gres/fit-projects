

#XQR:xhavle03
#
# Soubor: syntax.py
# Datum: 27. 3. 2014
# Autor: Vojtech Havlena, xhavle03
#
# Syntakticky analyzator pro 2. projekt do predmetu IPP
#

import xml.etree.ElementTree as ET
from lex import *
from xmloperation import *
from expr import *

# Chybove kody
EOK = 0
SYNERROR = 80
SEMERROR = 2
EFILEFORMAT = 4

# Informace o zpracovanem dotazu
qr = BaseQuery()

#
# Funkce, ktera reprezentuje netermina QUERY.
# @param lex Lexikalni analyzator
# @param tree Nacteny XML strom
# @return Dvojice (chybovy kod, seznam vyhovujicich elementu).
#
def Query(lex, tree):
	global qr
	lex.GetToken()
	if lex.token != TSELECT:
		return (SYNERROR, None)
	
	lex.GetToken()
	if lex.token != TELEMENT:
		return (SYNERROR, None)
	qr.select = lex.value;
	
	lex.GetToken()
	ret = Limit(lex)
	if ret != EOK:
		return (ret, None)
	
	if lex.token != TFROM:
		return (SYNERROR, None)
	lex.GetToken()
	ret = FromElm(lex)
	if ret != EOK:
		return (ret, None)
	
	result = GetXmlList(tree, qr);
	if lex.token == TWHERE:
		result = EvalueateCondition(lex, result)
		if result == None:
			return (SYNERROR, None)
		elif result == EFILEFORMAT:
			return (EFILEFORMAT, None)
	
	if lex.token != TEOF:
		return (SYNERROR, None)
	
	if qr.limit != None and qr.limit >= 0:
		result = result[0:qr.limit]
	
	return (EOK, result)
	
#
# Funkce, ktera reprezentuje neterminal LIMIT.
# @param lex Lexikalni analyzator
# @return Chybovy kod, resp. EOK
#
def Limit(lex):
	global qr
	if lex.token != TLIMIT:
		return EOK
	lex.GetToken()
	if lex.token != TNUMBER:
		return SYNERROR
	qr.limit = lex.value

	lex.GetToken()
	return EOK

#
# Funkce, ktera reprezentuje neterminal FROM.
# @param lex Lexikalni analyzator
# @return Chybovy kod, resp. EOK
#
def FromElm(lex):
	global qr
	if lex.token == TEOF or lex.token == TWHERE:
		qr.fromElem = None
		qr.fromAttr = None
		return EOK
	if lex.token == TROOT:
		qr.fromElem = "ROOT"
		lex.GetToken()
		return EOK
	elif lex.token == TELEMENT:
		qr.fromElem = lex.value
		lex.GetToken()
		if lex.token == TDOT:
			lex.GetToken()
			if lex.token == TELEMENT:
				qr.fromAttr = lex.value
				lex.GetToken()
				return EOK
			else:
				return SYNERROR
		else:
			return EOK
	elif lex.token == TDOT:
		lex.GetToken()
		if lex.token != TELEMENT:
			return SYNERROR
		qr.fromAttr = lex.value
		lex.GetToken()
		
		return EOK
	else:
		return SYNERROR
		
