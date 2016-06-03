
'''
 Projekt do predmetu SNT -- RCPSP
 Autor: Vojtech Havlena, xhavle03
 Datum: 2.4.2016
 Soubor: InputParser.py
 
 Trida slouzici pro parsovani vstupniho .sm souboru.
'''

from Job import Job

class InputParser:
	
	'''
	 Metoda, ktera parsuje nasledovniky z radku vstupniho souboru.
	 @param lines Radky souboru
	 @param index Index radku, kde zacina informace o nasledovnicich.
	 @return Nasledovnici aktivit.
	'''
	@staticmethod
	def ParseSuccessors(lines, index):
		succ = {}
		i = index
		while not(lines[i].startswith("***")):
			 spl = lines[i].split()
			 number = int(spl[0]) - 1
			 succNum = int(spl[2])
			 tmp = []
			 for j in range(0, succNum):
				 tmp.append(int(spl[3 + j]) - 1)
			 succ[number] = tmp
			 i = i + 1
		return succ	 
	
	'''
	 Metoda, ktera parsuje doby trvani aktivit z radku vstupniho souboru.
	 @param lines Radky souboru.
	 @param index Index radku, kde zacinaji doby trvani.
	 @return Doby trvani aktivit.
	'''
	@staticmethod
	def ParseDurations(lines, index):
		jobs = {}
		i = index
		while not(lines[i].startswith("***")):
			 spl = lines[i].split()
			 number = int(spl[0]) - 1
			 duration = int(spl[2])
			 tmp = []
			 for j in range(3, len(spl)):
				 tmp.append(int(spl[j]))
			 jobs[number] = Job(number, duration, tmp, [], [], 0)
			 i = i + 1
		return jobs	 
	
	'''
	 Metoda, ktera parsuje kapacity.
	 @param lines Radky souboru.
	 @param index Index radku, kde se nachazi informace o kapacitach.
	 @param Kapacity zdroju.
	'''
	@staticmethod
	def ParseCapacities(lines, index):
		return map(int, lines[index].split())

	'''
	 Metoda, ktera z primych nasledovniku spocita prime predchudce.
	 @param succ Nasledovnici
	 @return Predchudci.
	'''
	@staticmethod
	def MakePredFromSucc(succ):
		pred = {}
		for key, val in succ.iteritems():
			for v in val:
				if v in pred.keys():
					pred[v].append(key)
				else:
					pred[v] = [key]
		return pred
	
	'''
	 Metoda, ktera zpracuje a vytvori vstup v pozadovanych dat. strukturach.
	 @param Nasledovnici aktivit.
	 @param jobs Aktivity.
	 @return Trojice poli (nasledovnici, aktivity, doba trvani).
	'''	
	@staticmethod
	def CreateInput(succ, jobs):
		inp = []
		succRet = []
		dur = []
		pred = InputParser.MakePredFromSucc(succ)

		for i in range(0, len(jobs)):
			jb = jobs[i]
			if i in pred.keys():
				jb.predessors = set(pred[i])
			else:
				jb.predessors = set([])
			if i in succ.keys():
				jb.successors = succ[i]
			else:
				jb.successors = []
			inp.append(jb)
			succRet.append(succ[i])
			dur.append(jb.duration)
		return (succRet, inp, dur)
	
	'''
	 Metoda, ktera parsuje aktivity z radku vstupniho souboru.
	 @param lines Radky vstupniho souboru.
	 @return Ctverice poli (nasledovnici, aktivity, kapacity, doby tvrvani).
	'''
	@staticmethod
	def ParseJobInstance(lines):
		for i in range(0,len(lines)):
			if lines[i].startswith("PRECEDENCE RELATIONS"):
				succ = InputParser.ParseSuccessors(lines, i+2)
			if lines[i].startswith("REQUESTS/DURATIONS"):
				jbs = InputParser.ParseDurations(lines, i + 3)
			if lines[i].startswith("RESOURCEAVAILABILITIES"):
				cap = InputParser.ParseCapacities(lines, i + 2)
		s, i, dur = InputParser.CreateInput(succ, jbs)
		return (s, i, cap, dur) 
	
	'''
	 Metoda, ktera Parsuje vstupni soubor podle zadaneho jmena.
	 @param fname Jmeno .sm souboru
	 @return Ziskane informace o projektu.
	'''
	@staticmethod
	def ParseInput(fname):
		with open(fname) as f:
			content = f.readlines()
			return InputParser.ParseJobInstance(content)
			
	
