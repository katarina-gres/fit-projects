
'''
 Projekt do predmetu SNT -- RCPSP
 Autor: Vojtech Havlena, xhavle03
 Datum: 2.4.2016
 Soubor: SolutionValidity.py
 
 Trida slouzici pro overeni spravnosti vytvoreneho rozvrhu a
 trida pro vypocet dolniho ohraniceni kriticke cesty.
'''

from SerialScheduleScheme import SerialSchedule

class SolutionValidity(object):
	
	'''
	 Konstruktor.
	 @param scheme Trida pro seriove schema generovani rozvrhu.
	'''
	def __init__(self, scheme):
		self.__scheme = scheme
	
	'''
	 Metoda, ktera zkontroluje, zda nalezene reseni splnuje omezeni na priority.
	 @param sol Kontrolovane reseni.
	 @return True -- splnuje omezeni na priority, jinak False.
	'''
	def IsOrderPreserved(self, sol):
		visited = {}
		for job in sol:
			visited[job.number] = job.finish
			
		for i in range(0, self.__scheme.GetN()+2):
			if not(i in visited.keys()):
				return False
		
		for job in sol:
			start = job.finish - job.duration
			for pre in job.predessors:
				if start < visited[pre]:
					return False
		
		return True
		
	
	'''
	 Metoda, ktera kontroluje, zda je zadany rozvrh realizovatelny.
	 @param sol Reseni.
	 @param capacities Kapacity zdroju.
	 @return True -- je realizovatelne, jinak False.
	'''	
	def IsSolutionFeasible(self, sol, capacities):
		finTimes = self.__scheme.FinishTimes(sol)
		capRange = range(0, len(capacities))
		remCap = self.__scheme.RemainingCapacitiesCore(sol, finTimes, capRange)
		for k,v in remCap.iteritems():
			if v < 0:
				return False
		
		return self.IsOrderPreserved(sol)


class CriticalPath(object):
	
	'''
	 Konstruktor.
	 @param succ Nasledovnici aktivit.
	 @param dur Trvani aktivit.
	'''
	def __init__(self, succ, dur):
		self._pred = {}
		self._dur = dur
		self._succ = succ
		for k in range(0, len(succ)):
			for v in succ[k]:
				if v in self._pred.keys():
					self._pred[v].append(k)
				else:
					self._pred[v] = [k]
		
	
	'''
	 Metoda, ktera topologicky usporada zadany graf.
	 @param graph Graf, ktery se ma topologicky usporadat.
	 @return Seznam vrcholu v topologickem usporadani.
	'''
	@staticmethod
	def TopologicalSort(graph):
		L = []
		color = {}
		for u in graph:
			color[u] = "w"
		ret = True
		for u in graph:
			if color[u] == "w":
				ret = CriticalPath.DFSVisit(graph, u, color, L)
			if not(ret):
				return []
		L.reverse()
		return L
 
	'''
	 Metoda, ktera provadi prochod grafem do hloubky.
	 @param graph Prochazeny graf.
	 @param u Aktualni vrchol
	 @param color Obarveni vrcholu.
	 @return True -- neobsahuje cyklus, jinak False.
	'''
	@staticmethod
	def DFSVisit(graph, u, color, L):
		color[u] = "g"
		ret = True
		for v in graph[u]:
			if color[v] == "g":
				return False
			if color[v] == "w":
				ret = CriticalPath.DFSVisit(graph, v, color, L)
			if not(ret):
				return False
		color[u] = "b"
		L.append(u) 
		return True
	
	
	'''
	 Metoda, ktera spocita dolni ohraniceni kriticke cesty.
	 @return LB CP.
	'''
	def ComputeCRLB(self):
		graph = dict()
		for k in range(0, len(self._succ)):
			graph[k] = self._succ[k]
			
		rSort = CriticalPath.TopologicalSort(graph)
		dist = dict()
		for vertex in rSort:
			dist[vertex] = 0
			if not(vertex in self._pred.keys()):
				continue
			for u in self._pred[vertex]:
				dist[vertex] = max(dist[vertex], dist[u] + self._dur[vertex])
		return max(dist.values())

