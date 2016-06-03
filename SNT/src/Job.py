
'''
 Projekt do predmetu SNT -- RCPSP
 Autor: Vojtech Havlena, xhavle03
 Datum: 2.4.2016
 Soubor: Job.py
 
 Trida slouzici pro reprezentaci aktivity.
'''

class Job:
	'''
	 Konstruktor.
	 @param number Cislo aktivity
	 @param duration Doba trvani
	 @param resources Pozadovane zdroje aktivity
	 @param predessors Primi predchudci aktivity
	 @param succ Primi nasledovnici aktivity
	 @param finish Doba ukonceni aktivity
	'''
	def __init__(self, number, duration, resources, predessors, succ, finish):
		self.number = number
		self.duration = duration
		self.resources = resources
		self.predessors = set(predessors)
		self.finish = finish
		self.successors = set(succ)
	
	'''
	 Porovnani na rovnost (podle cisla aktivity).
	'''
	def __eq__(self, other): 
		return self.number == other.number
	
	'''
	 Hesovani, podle cisla aktivity.
	'''
	def __hash__(self):
		return hash(self.number)
	
	'''
	 Prevod aktivity na retezec.
	'''
	def __str__(self):
		return "Cislo %s, trvani %s, zdroje %s, predchudci %s, konec %s\n" % (self.number, self.duration, self.resources, self.predessors, self.finish)
	
	'''
	 Prevod aktivity na retezec.
	'''
	def __repr__(self):
		return "Cislo %s, trvani %s, zdroje %s, predchudci %s, konec %s\n" % (self.number, self.duration, self.resources, self.predessors, self.finish)
		

