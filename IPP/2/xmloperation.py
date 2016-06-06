

#XQR:xhavle03
#
# Soubor: xmloperation.py
# Datum: 27. 3. 2014
# Autor: Vojtech Havlena, xhavle03
#
# Operace pro praci s XML pro 2. projekt do predmetu IPP
#

import xml.etree.ElementTree as ET

#
# Trida, ktera reprezentuje hlavni parametry dotazu.
#
class BaseQuery:
	select = None
	fromElem = None
	fromAttr = None
	limit = None

#
# Funkce, ktera nalezne element from.
# @param root Korenovy element.
# @param element Hledany element.
# @param attr Hledany atribut.
# @return Hledany element splnujici pozadavky from.
#	
def FindFrom(root, element, attr):
	if root == None:
		return None
	if element != None and attr != None:
		if root.tag == element and root.get(attr) != None:
			return root
	elif element != None and attr == None:
		if root.tag == element:
			return root
	elif element == None and attr != None:
		if root.get(attr) != None:
			return root
			
	for el in list(root):
		ret = FindFrom(el, element, attr)
		if ret != None:
			return ret

#
# Funkce vybirajici elementy, ktere splnuji polozku select.
# @param tree XML strom.
# @param qr Parametry, podle kterych se ma vyji vybirat elementy.
# @return Seznam splnenych elementu.
#	
def GetXmlList(tree, qr):
	if qr.fromElem == None and qr.fromAttr == None:
		return list()
	if qr.fromElem == "ROOT":
		newRoot = ET.Element("ROOT")
		newRoot.append(tree.getroot())
		
		tree = ET.ElementTree(newRoot)
		foundElement = tree.getroot()
	else:
		root = tree.getroot()
		foundElement = FindFrom(root, qr.fromElem, qr.fromAttr)	
	
	if foundElement == None:
		return list()
	retList = list(foundElement.iter(qr.select));
	if qr.select == foundElement.tag:
		retList += foundElement
	#if qr.limit != None:
	#	retList = retList[0:qr.limit]
	
	return retList

#
# Funkce, ktera provede sjednoceni dvou seznamu.
# @param l1 Prvni seznam.
# @param l2 Druhy seznam.
# @return Sjednoceni l1 a l2.
#
def GetListUnion(l1, l2):
	ret = list()
	for item in l1:
		if not item in l2:
			ret.append(item)
	ret += l2
	return ret

#
# Funkce, ktera provede prunik dvou seznamu.
# @param l1 Prvni seznam.
# @param l2 Druhy seznam.
# @return Prunik l1 a l2.
#	
def GetListIntersection(l1, l2):
	ret = list()
	for item in l1:
		if item in l2:
			ret.append(item)
	return ret

#
# Funkce, ktera provede rozdil dvou seznamu.
# @param l1 Prvni seznam.
# @param l2 Druhy seznam.
# @return Rozdil l1 a l2.
#	
def GetListDifference(l1, l2):
	ret = list()
	for item in l1:
		if not item in l2:
			ret.append(item)
	return ret

#
# Funkce, ktera vrati hodnotu elementu/atributu.
# @param element Element.
# @param attr Attribut.
# @return Pozadovana hodnota.
#
def GetElementValue(element, attr):
	if attr != None:
		return element.get(attr)
	else:
		if len(list(element)) != 0:
			return None
		return element.text
		
