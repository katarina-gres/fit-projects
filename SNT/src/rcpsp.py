
'''
 Projekt do predmetu SNT -- RCPSP
 Autor: Vojtech Havlena, xhavle03
 Datum: 2.4.2016
 Soubor: rcpsp.py
 
 Hlavni soubor programu, obsahuje parametry optimalizace a hlavni smycku
 ACO algoritmu.
'''

from random import uniform
from copy import deepcopy
import cProfile
import sys

from Job import Job
from InputParser import InputParser
from SerialScheduleScheme import SerialSchedule
from LocalOptimisation import SerialScheduleLocalOpt
from SolutionValidity import SolutionValidity, CriticalPath

# Ma se vypisovat podrobnejsi statistika?
statistics = False
# Ma se pouzit lokalni optimalizacni strategie.
localOptimisation = True

# Parametr c (kombinace primeho a souctoveho vyhodnoceni).
cParam = 0.5
# Alpha, Beta pro vyhodnocovani pravdepodobnosti p_{ij} (vliv 
# heuristicke informace).
alpha = 1.0
beta = 1.0
# Pocet provedenych generaci.
generations = 20

'''
 Funkce implementujici cely ACO algoritmus pro optimalizacni problem 
 planovani rozvrhu.
 @param start Aktivita zahajeni projektu.
 @param jobs Vsechny aktivity projektu.
 @param capacity Kapacita zdroju.
 @param succ Primy nasledovnici jednotlivych aktivit.
 @param dur Doba tvrvani jednotlivych aktivit.
 @return Hodnota makespan nejlepsiho nalezeneho reseni.
'''
def AntCycle(start, jobs, capacity, n, succ, dur):
	global generations
	c = 1.0
	tau = [[c for x in range(n+2)] for x in range(n+2)]
	mi = [[c for x in range(n+2)] for x in range(n+2)]
	antCount = 5
	ro = 0.02
	pstForget = 0.01
	
	serialSchedule = SerialScheduleLocalOpt(start, jobs, capacity, n, dur, succ, cParam, alpha, beta)
	validator = SolutionValidity(serialSchedule)

	bestAllTime = bestSoFarFit = 2000
	bestAllTimeRes = bestSoFarRes = []
	sumAll = 0
	loCounter = 0
	
	#Hlavni smycka pro kazdou generaci.
	for gen in range(generations):
		bestFitness = 2000
		bestRes = []
		serialSchedule.entropies.clear()
		for m in range(0, antCount):
			res, ret = serialSchedule.ProcessScheme(mi, tau) 
			if res <= bestFitness:
				bestFitness = res
				bestRes = deepcopy(ret)
			serialSchedule.ResetJobs()

		#Lokalni optimalizacni strategie
		loCounter = loCounter + 1
		if localOptimisation and loCounter % 20 == 0:
			loCounter = 0
			a, b = serialSchedule.LocalOptimisationStrategy( bestRes, bestFitness)
			if b < bestFitness:
				bestRes = deepcopy(a)
				if statistics:
					print "Pred tim: ", bestFitness, "potom: ", b
				bestFitness = b
		
		#Aktualizace hodnot tau		
		for i in range(0,n+1):
			for j in range(0,n+1):
				tau[i][j] = (1.0 - ro) * tau[i][j]
		if bestSoFarRes != []:
			tau = serialSchedule.UpdateTau(tau, bestSoFarRes, bestSoFarFit,ro)
		tau = serialSchedule.UpdateTau(tau,bestRes, bestFitness,ro)
		
		sumAll = sumAll + bestFitness
		
		if bestFitness <= bestAllTime:
			bestAllTime = bestFitness
			bestAllTimeRes = deepcopy(bestRes)
		
		#Aplikace zapominani
		forget = uniform(0.0, 1.0)
		if bestFitness <= bestSoFarFit or forget < pstForget:
			bestSoFarFit = bestFitness
			bestSoFarRes = deepcopy(bestRes)
	
	#Vypocet entropie		
	#for k in range(1, n):
	#	print k, serialSchedule.entropies[k] / float(m)
	
	if not(validator.IsSolutionFeasible(bestAllTimeRes, capacity)):
		return -1
	if statistics:
		print bestAllTimeRes, len(bestAllTimeRes)
		print "Prumer: ", float(sumAll) / generations, "soucet, ", sumAll
		print validator.IsSolutionFeasible(bestAllTimeRes, capacity)
	return bestAllTime

'''
 Vstupni funkce programu. Provadi zpracovani .sm souboru a ridi
 optimalizaci pomoci ACO.
'''
def main():
	if len(sys.argv) != 3:
		print "Spatne parametry programu. "
		print "Program ocekava 1. parametr soubor s instanci RCPSP (.sm) a 2. parametr pocet aktivit v dane instanci (bez aktivity zahajeni a ukonceni projektu)."
		exit(1)
	
	fileName = sys.argv[1]
	n = int(sys.argv[2])
	try:
		succ, jbs, cap, dur = InputParser.ParseInput(fileName)
	except:
		print "Chyba pri zpracovavani vstupniho souboru."
		exit(2)
	
	#cp = CriticalPath(succ, dur);
	#print fileName, ": ", cp.ComputeCRLB()
	
	bestMakespan = 0
	try:
		bestMakespan = AntCycle(jbs[0], jbs, cap, n, succ, dur)
	except:
		print "Chyba pri hledani reseni, zkontrolujte parametry."
		exit(2)
		
	if bestMakespan == -1:
		print "Chyba, nalezene reseni nesplnuje omezujici podminky"
		exit(2)
	print fileName, ": ", bestMakespan
	
	
if __name__ == "__main__":
	main()
