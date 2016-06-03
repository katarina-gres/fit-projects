
'''
 Projekt do predmetu SNT -- RCPSP
 Autor: Vojtech Havlena, xhavle03
 Datum: 2.4.2016
 Soubor: LocalOptimisation.py
 
 Trida implementujici lokalni optimalizacni strategii.
'''

from SerialScheduleScheme import SerialSchedule
from copy import deepcopy
from Job import Job

class SerialScheduleLocalOpt(SerialSchedule):
	
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
		super(SerialScheduleLocalOpt, self).__init__(start, jobs, capacity, n, dur, succ, c, alpha, beta)
		self._allPredessors = {}
		self.ComputeAllPredessors()
		
	'''
	 Metoda, ktera provadi seriove generovani rozvrhu podle zadaneho reseni.
	 @param sol Reseni, podle ktereho se ma provest nove generovani.
	 @param startAt Na jake pozici v seznamu aktivit se ma zacit generovat.
	 @return Dvojice (makespa, rozvh) nejlepsiho nalezeneho reseni.
	'''
	def OptSSGS(self, sol, startAt):
		self._actives.clear()
		self._capacities.clear()
		jobsSet = set(self._jobs)
		scheduled = set([self._start])
		rangeN = range(0,self._n+1)
		
		capRange = range(0, len(self._capacity))	
		
		scheduledIndex = set([0])
		ret = [self._start]
		for g in range(1,self._n+2):
			if g < startAt:
				selected = self._jobs[sol[g].number]
				selected.finish = sol[g].finish
				ret.append(selected)
				scheduledIndex.add(selected.number)
				scheduled.add(selected)
				super(SerialScheduleLocalOpt, self).UpdateUsedCapacities(selected, capRange)
				continue
			
			
			elSet = super(SerialScheduleLocalOpt, self).EligibleSet(jobsSet, scheduled, scheduledIndex)
			finTime = super(SerialScheduleLocalOpt, self).FinishTimes(scheduled)

			selected = self._jobs[sol[g].number]
			
			if not(selected in elSet):
				return (None, None)

			efj = super(SerialScheduleLocalOpt, self).EF(selected) + selected.duration
			fin = super(SerialScheduleLocalOpt, self).Finish(selected, efj, finTime, scheduled) + selected.duration
			selected.finish = fin
			
			scheduled.add(selected)
			super(SerialScheduleLocalOpt, self).UpdateUsedCapacities(selected, capRange)
				
			scheduledIndex.add(selected.number)
			ret.append(selected)
			
		res = 0
		for h in self._jobs:
			res = max(h.finish, res)
		if self._jobs[self._n+1].finish != res:
			self._jobs[self._n+1].finish = res
		return (res, ret)
		
	
	'''
	 Metoda, ktera spocita mnozinu vsech predchudcu dane aktivity.
	 @param job Aktivita, jejiz predchudci se maji spocitat.
	 @return Vsichi predchudci dane aktivity.
	'''
	def SinglePredessor(self, job):
		ret = job.predessors
		for item in job.predessors:
			ret = ret | self.SinglePredessor(self._jobs[item])
		return ret
		
	'''
	 Metoda, ktera spocita vsechny predchudce vsech aktivit.
	'''
	def ComputeAllPredessors(self):
		for job in self._jobs:
			self._allPredessors[job.number] = self.SinglePredessor(job)
	
	'''
	 Metoda, ktera zjistuje, zda je mozne prohodit dve aktivity v rozvrhu.
	 @param sol Reseni (rozvrh)
	 @param i Index 1. aktivity.
	 @param j Index 2. aktivity.
	 @return True -- je mozne prohodit, jinak False.
	'''
	def CanSwap(self, sol, i, j):
		iNumber = sol[i].number
		jNumber = sol[j].number
		
		if iNumber in self._allPredessors[jNumber]:
			return False
		k = 0
		p = set()
		while k < i:
			p.add(sol[k].number)
			k = k + 1  
			
		if not(self._allPredessors[jNumber] <= p):
			return False
		return True
		
	'''
	 Metoda, ktera provadi vypocet lokalni optimalizacni strategie.
	 @param sol Nalezene reseni, ktere se ma optimalizovat.
	 @param bestFit Makespan optimalizovaneho reseni.
	 @return Dvojice (reseni, makespan) optimalizovaneho reseni.
	'''
	def LocalOptimisationStrategy(self, sol, bestFit):
		limit = len(sol) - 3
		rng = range(1, limit)
		
		bestFound = []
		bestFitness = bestFit
		newFitness = 0

		actSol = sol
		bestSol = deepcopy(sol)
		for i in rng:
			for j in range(i + 1, limit):
				indexI = bestSol.index(Job(i, 0, [], [], [], 0))
				indexJ = bestSol.index(Job(j, 0, [], [], [], 0))
				if self.CanSwap(bestSol, indexI, indexJ):
					bestSol[indexI], bestSol[indexJ] = bestSol[indexJ], bestSol[indexI]
				else:
					continue

				super(SerialScheduleLocalOpt, self).ResetJobs()
				newFitness, actSol = self.OptSSGS(bestSol, min(indexI, indexJ))
				bestSol[indexJ], bestSol[indexI] = bestSol[indexI], bestSol[indexJ]

				if newFitness == None:
					continue
				if newFitness < bestFitness:
					bestSol = deepcopy(actSol)
					bestFitness = newFitness
		return (bestSol, bestFitness)
