
'''
 Projekt do predmetu SNT -- RCPSP
 Autor: Vojtech Havlena, xhavle03
 Datum: 2.4.2016
 Soubor: SerialScheduleScheme.py
 
 Trida implementujici Seriove generovani rozvrhu s vyuzitim optimalizace
 pomoci kolonie mravencu.
'''

from math import pow, log
from copy import deepcopy
from random import randint, uniform

# Maximalni velikost timespanu projektu.
maxT = 400

class SerialSchedule(object):
	
	'''
	 Konstruktor.
	 @param start Pocatecni aktivita projektu.
	 @param jobs Aktivity projektu.
	 @param capacity Kapacity zdroju.
	 @param n Pocet aktivit.
	 @param dur Doba trvani aktivit.
	 @param succ Primi nasledovnici vsech aktivit.
	 @param c Hodnota c (kombinace primeho vyhodnoceni a souhrnneho)
	 @param alpha, beta Vlivy heuristiky.
	'''
	def __init__(self, start, jobs, capacity, n, dur, succ, c, alpha, beta):
		self._start = start
		self._jobs = jobs
		self._capacity = capacity
		self._n = n
		self._dur = dur
		self._succ = succ
		self._actives = {}
		self._capacities = {}
		self.entropies = {}
		self._c = c
		self._alpha = alpha
		self._beta = beta
		
		self._lfs = {}
		for item in self._jobs:
			self._lfs[item.number] = self.LF(item.number)
			
		global maxT
		maxT = 0
		for job in self._jobs:
			maxT = maxT + job.duration
	
	'''
	 Metoda, ktara vrati pocet aktivit v projektu.
	 @return Pocet aktivit projektu.
	'''
	def GetN(self):
		return self._n
			
	'''
	 Metoda, ktera implementuje seriove generovani rozvrhu. Pro obecnejsi pouziti
	 se zarazuje do rozvrhu i aktivita ukonceni projektu.
	 @param mi Matice heuristickych hodnot.
	 @param tau Matice feromonovych hodnot.
	 @return Dvojice (Makespan, rozvrh) nejlepsiho reseni.
	'''
	def ProcessScheme(self, mi, tau):
		self._actives.clear()
		self._capacities.clear()
		jobsSet = set(self._jobs)
		scheduled = set([self._start])
		rangeN = range(0,self._n+1)
			
		capRange = range(0, len(self._capacity))
	
		scheduledIndex = set([0])
		ret = [self._start]
		for g in range(1, self._n+2):
			elSet = self.EligibleSet(jobsSet, scheduled, scheduledIndex)
			finTime = self.FinishTimes(scheduled)

			mi = self.UpdateMi(elSet, mi, rangeN)
			elList = list(elSet)
			selected = self.ChooseFromElSet(g, elList, mi, tau)

			efj = self.EF(selected) + selected.duration
			fin = self.Finish(selected, efj, finTime, scheduled) + selected.duration
			selected.finish = fin
			
			scheduled.add(selected)
			self.UpdateUsedCapacities(selected, capRange)
			
			scheduledIndex.add(selected.number)
			ret.append(selected)
			
		res = 0
		for h in self._jobs:
			res = max(h.finish, res)
		if self._jobs[self._n+1].finish != res:
			self._jobs[self._n+1].finish = res
		return (res, ret)
	
	'''
	 Metoda, ktera aktializuje matici heuristickych hodnot.
	 @param elSet Mnozina vhodnych aktivit.
	 @param mi Matice heuristikych hodnot.
	 @param rangeN Rozsah hodnot N.
	 @return Aktualizovana hodnota matice mi.
	'''
	def UpdateMi(self, elSet, mi, rangeN):
		maxk = 0
		for item in elSet:
			maxk = max(maxk, self._lfs[item.number])
		
		nw = []
		for j in rangeN:
			nw.append(maxk - self._lfs[j] + 1)
		for i in rangeN:
			mi[i] = nw
		return mi

	'''
	 Metoda, ktera resetuje zjistene koncove casy aktivit z predchoziho
	 planovani.
	'''
	def ResetJobs(self):
		for j in self._jobs:
			j.finish = 0
		
	'''
	 Metoda, ktera provede vyber aktivity z mnoziny vhodnych aktivit.
	 @param place Misto v posloupnosti SGS, kam se vybira aktivita.
	 @param elList Mnozina vhodnych aktivit.
	 @param mi Heuristicke informace.
	 @param tau Feromonova informace.
	 @return Vybrana aktivita z mnoziny vhodnych aktivit.
	'''
	def ChooseFromElSet(self, place, elList, mi, tau):
		pst = uniform(0.0, 1.0)
		accum = 0.0
		denominator = 0

		xi = 0.0
		yi = 0.0
		for item in elList:
			for k in range(1, place+1):
				xi = xi + tau[k][item.number]
			yi = yi + tau[place][item.number]
		
		for item in elList:
			nom = 0.0
			for k in range(1,place+1):
				nom = nom + tau[k][item.number]
			nom = nom * (1.0-self._c)*yi + self._c*xi*tau[place][item.number]
			
			denominator += pow(nom,self._alpha)*pow(mi[place][item.number],self._beta)
		i = 0
		
		#Vypocet hodnoty entropie
		'''entropy = 0.0
		sumpst = 0.0
		for it in elList:
			nom = 0.0 
			for k in range(1,place+1):
				nom = nom + tau[k][it.number]
			nom = nom * (1.0-self._c)*yi + self._c*xi*tau[place][it.number]
			
			pij = pow(nom,self._alpha)*pow(mi[place][it.number],self._beta) / denominator
			entropy = entropy + pij*log(pij)
			sumpst = sumpst + pij

		
		if not(place in self.entropies.keys()):
			self.entropies[place] = -entropy
		else:
			self.entropies[place] = self.entropies[place] + -entropy'''
		
		while pst >= 0.0:
			item = elList[i]
			nom = 0.0 
			for k in range(1,place+1):
				nom = nom + tau[k][item.number]
			nom = nom * (1.0-self._c)*yi + self._c*xi*tau[place][item.number]
			
			pij = pow(nom,self._alpha)*pow(mi[place][item.number],self._beta) / denominator
			pst = pst - pij
			i = i + 1
		return elList[i - 1]
	
	'''
	 Metoda, ktera rekurzivne spocita hodnotu LF -- latest finish time.
	 @param index Index (cislo) aktivity, jejiz hodnota LF se ma spocitat.
	 @return LF aktivity s cislem index.
	'''
	def LF(self, index):
		ret = []
		if index == self._n+1:
			return maxT
		for i in self._succ[index]:
			ret.append(self.LS(i))
		return min(ret)
	
	'''
	 Metoda, ktera spolu s metodou LF rekurzivne pocita hodnotu latest finish time
	 dane aktivity.
	 @param index Index (cislo) aktivity.
	 @return Hodnota LS aktivity s cislem index.
	'''
	def LS(self, index):
		if index == self._n+1:
			return maxT
		return self.LF(index) - self._dur[index]
	
	'''
	 Metoda, ktera spocita mnozinu vhodnych aktivit.
	 @param jobs Mnozina aktivit.
	 @param scheduled Mnozina jiz naplanovanych aktivit.
	 @param scheduledIndex Mnozina indexu jiz naplanovanych aktivit.
	 @return Mnozina vhodnych aktivit.
	'''
	def EligibleSet(self, jobs, scheduled, scheduledIndex):
		ret = set()
		al = jobs - scheduled
		for item in al:
			if item.predessors <= scheduledIndex:
				ret.add(item)
		return ret
	
	'''
	 Metoda, ktera zjisti casy ukonceni naplanovanych aktivit.
	 @param scheduled Naplanovane aktivity.
	 @return Mnozina casu ukonceni napl. aktivit.
	'''
	def FinishTimes(self, scheduled):
		ret = set()
		for item in scheduled:
			ret.add(item.finish)
		return ret
		
	'''
	 Metoda, ktera zjisti aktivity, ktere jsou aktivni v danych casech.
	 @param scheduled Mnozina jiz naplanovanych aktivit.
	 @param time Mnozina casu.
	 @return Mnozina aktivnich (provadenych) aktivit.
	'''
	def ActiveJobs(self, scheduled, time):
		ret = set()
		for item in scheduled:
			if (item.finish - item.duration <= time) and (time < item.finish):
				ret.add(item)
		return ret

	'''
	 Metoda, ktera zjisti aktivity, ktere jsou aktivni v danych casech (s vyuzitim 
	 hesovanych hodnot).
	 @param scheduled Mnozina jiz naplanovanych aktivit.
	 @param time Mnozina casu.
	 @return Mnozina aktivnich (provadenych) aktivit.
	'''
	def ActiveJobsSet(self, scheduled, time):
		if time in self._actives.keys():
			return self._actives[time]
		else:
			return []
			
	'''
	 Metoda, ktera spocita zbyvajici kapacity zdroju v danych casech (s vyuzitim 
	 optimalizace pomoci hesovani hodnot).
	 @param scheduled Mnozina jiz naplanovanych aktivit.
	 @param finTime Mnozina casu.
	 @param capRange Rozmezi indexu kapacit.
	 @return Pole obsahujici zbyvajici kapacity.
	'''	
	def RemainingCapacities(self, scheduled, finTime, capRange):
		ret = []
		i = 0
		for t in finTime:
			try:
				start =  self._capacities[t]
			except KeyError:
				start = self._capacity[:]
				self._capacities[t] = start
				
			ret.append(start)
		return ret
	
	'''
	 Metoda, ktera spocita zbyvajici kapacity zdroju v danych casech (primo, bez vyuziti 
	 optimalizace pomoci hesovani hodnot).
	 @param scheduled Mnozina jiz naplanovanych aktivit.
	 @param finTime Mnozina casu.
	 @param capRange Rozmezi indexu kapacit.
	 @return Pole obsahujici zbyvajici kapacity.
	'''	
	def RemainingCapacitiesCore(self, scheduled, finTime, capRange):
		ret = {}
		i = 0
		for t in finTime:
			act = self.ActiveJobs(scheduled, t)
			for k in capRange:
				start = self._capacity[k]
				for j in act:
					start = start - j.resources[k]
				ret[(t,k)] = start
		return ret
	
	'''
	 Metoda, ktera spocita EF (earliest finish) pro zvolenou aktivitu.
	 @param selected Zvolena aktivita.
	 @return EF aktivity selected.
	'''
	def EF(self, selected):
		m = 0
		for h in list(selected.predessors):
			m = max(self._jobs[h].finish, m)
		return m
	
	'''
	 Metoda, ktera vypocita cas ukonceni aktivity.
	 @param selected Aktivita, jejiz cas ukonceni se ma spocitat.
	 @param ef EF zvolene aktivity.
	 @param finTime Mnozina casu, ve kterych jsou dokonceny aktivity.
	 @param scheduled Mnozina naplanovanych aktivit.
	 @return Cas ukonceni aktivity.
	'''
	def Finish(self, selected, ef, finTime, scheduled):
		ret = set()
		lf = self._lfs[selected.number] 
		
		rng = set(range(ef - selected.duration, lf - selected.duration + 1)) & finTime
		capRange = range(0, len(self._capacity))
		rng = sorted(rng)
		for t in rng:
			add = True
			tauSet = set(range(t,t+selected.duration)) & finTime
			for tau in tauSet:
				tmp = self.RemainingCapacities(scheduled, set([tau]), capRange)
				for k in capRange: 
					if selected.resources[k] > tmp[0][k]:
						add = False
						break
				if not(add):
					break
			if add:
				return t
		return min(ret)
	
	'''
	 Metoda, ktera aktualizuje hesovane hodnoty pro vypocitani zbyvajicich kapacit 
	 zdroju v danem case.
	 @param selected Zvolena aktivita, jejiz hodnoty se maji aktualizovat.
	 @param capRange Rozsah indexu kapacit.
	'''
	def UpdateUsedCapacities(self, selected, capRange):
		for t in range(selected.finish - selected.duration, selected.finish):
			store = []
			try:
				store = self._capacities[t]
			except KeyError:
				store = self._capacity[:]
			for k in capRange:
				store[k] = store[k] - selected.resources[k]
			self._capacities[t] = store
		
	'''
	 Metoda, ktera aktualizuje hodnotu feromonove matice tau.
	 @param tau Feromonova matice tau.
	 @param res Nalezeny rozvrh (reseni)
	 @param makeSpan Makespan nalezeneho reseni.
	 @param ro Hodnota vyparovani feromonove informace.
	'''
	def UpdateTau(self, tau,res,makeSpan,ro):
		i = 0
		for item in res:
			tau[i][item.number] = tau[i][item.number] + ro*1.0/(2.0*makeSpan)
			i = i + 1
		return tau	

