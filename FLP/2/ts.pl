/**
 * Projekt c. 2 do predmetu FLP.
 * Nazev projektu: Turinguv stroj
 * Autor: Vojtech Havlena, xhavle03 
 */


:- dynamic transition/4.

/**
 * Predikat pro zjisteni symbolu na pasce nachazejiciho se na dane pozici.
 * symbolAtPos(Pos, P, Sym)
 * @param Pos Pozice na pasce.
 * @param P Paska (seznam symbolu)
 * @param Sym Symbol na dane pozici.
 */
symbolAtPos(_, [], ' ').
symbolAtPos(0, [H|_], Sym) :- H = Sym, !.
symbolAtPos(Pos, [_|C], Sym) :- 
	Pos1 is Pos - 1, 
	symbolAtPos(Pos1, C, Sym).

/**
 * Predikat pro nahrazeni symbolu na dane pozici pasky (v pripade, ze je nahrazovan symbol 'mimo' pasku, 
 * jsou doplneny prazdne symboly ' ').
 * replaceAtPos(Pos, P1, Sym, P2)
 * @param Pos Pozice na pasce.
 * @param P1 Zdrojova paska.
 * @param Sym Symbol, ktery ma byt vlozen na misto symbolu na dane pozici.
 * @param P2 Cilova paska.
 */
replaceAtPos(0, [], Sym, [Sym|[]]). 
replaceAtPos(Pos, [], Sym, [' '|TR]) :- 
	Pos \= 0, P1 is Pos - 1, 
	replaceAtPos(P1, [], Sym, TR).
replaceAtPos(0, [_|T], Sym, [HR|TR]) :- HR = Sym, T = TR, !.
replaceAtPos(Pos, [H|T], Sym, [HR|TR]) :- H = HR, 
	P1 is Pos - 1, 
	replaceAtPos(P1, T, Sym, TR).

/**
 * Predikat pro provedeni prechodu Turingova stroje.
 * makeAction(R, Conf, Pos, NewPos, ResConf)
 * @param R Akce, ktera se ma provest.
 * @param Conf Aktualni obsah pasky.
 * @param Pos Aktualni pozice na pasce (pred provedenim kroku).
 * @param NewPos Pozice hlavy na pasce po provedeni akce.
 * @param ResConf Obsah pasky po provedeni akce.
 */
makeAction('R', Conf, Pos, NewPos, ResConf) :- 
	NewPos is Pos + 1, 
	(length(Conf,NewPos) -> append(Conf, [' '], ResConf); Conf = ResConf).
makeAction('L', Conf, Pos, NewPos, ResConf) :- 
	NewPos is Pos - 1, 
	Conf = ResConf.
makeAction(A, Conf, Pos, NewPos, ResConf) :- 
	A \= 'R', A \= 'L', 
	Pos = NewPos, 
	replaceAtPos(Pos, Conf, A, ResConf).

/**
 * Predikat pro provedeni 1 kroku (prechodove funkce) Turingova stroje. Je kontrolovano, jestli nebyl 
 * dosazen koncovy stav a jestli nedoslo k prepadnuti hlavy.
 * step(Conf, State, Pos, NewState, NewPos, ResConf)
 * @param Conf Aktualni obsah pasky.
 * @param State Aktualni stav (pred provedenim kroku)
 * @param Pos Aktualni pozice na pasce (pred provedenim kroku).
 * @param NewState Novy stav po provedeni kroku.
 * @param NewPos Pozice hlavy na pasce po provedeni kroku.
 * @param ResConf Obsah pasky po provedeni kroku.
 */
step(Conf, State, Pos, NewState, NewPos, ResConf) :- 
	State \= 'F', %not(State = stateF), 
	Pos >= 0, symbolAtPos(Pos, Conf, Sym), transition(State, Sym, NewState, Act), 
	makeAction(Act, Conf, Pos, NewPos, ResConf), NewPos >= 0.

/**
 * Predikat pro simulovai cest, ktere maji presne danou delku.
 * simulateLenTS(Conf, State, Pos, Len, NewState, NewPos, ResConf)
 * @param Conf Aktualni obsah pasky.
 * @param State Aktualni stav (pred provedenim vypoctu)
 * @param Pos Aktualni pozice na pasce (pred provedenim vypoctu).
 * @param Len Pocet provedenych kroku.
 * @param NewState Novy stav po provedeni vypoctu dane delky.
 * @param NewPos Pozice hlavy na pasce po provedeni vypoctu dane delky.
 * @param ResConf Obsah pasky po provedeni vypoctu dane delky.
 */
simulateLenTS(Conf, State, Pos, 0, State, Pos, Conf) :- !.
simulateLenTS(Conf, State, Pos, Len, NewState, NewPos, ResConf) :- 
	Len > 0,	
	Len1 is Len - 1,
	step(Conf, State, Pos, S1, P1, R1), 
	simulateLenTS(R1, S1, P1, Len1, NewState, NewPos, ResConf).

/**
 * Predikat pro simulovani vypoctu Turingova stroje. Je vyuzivano DLS.
 * simulateAll(Conf, State, Pos, Len, NewState, NewPos, ResConf)
 * @param Conf Aktualni obsah pasky.
 * @param State Aktualni stav (pred provedenim vypoctu)
 * @param Pos Aktualni pozice na pasce (pred provedenim vypoctu).
 * @param Len Pocet provedenych kroku.
 * @param NewState Novy stav po provedeni vypoctu dane delky.
 * @param NewPos Pozice hlavy na pasce po provedeni vypoctu dane delky.
 * @param ResConf Seznam konfiguraci (trojice stav, pozice, obsah pasky), ktere byly 
 * postupne pouzity behem vypoctu.
 */
simulateAll(Conf, State, Pos, NewState, NewPos, ResConf) :- 
	simulateAll(Conf, State, Pos, NewState, NewPos, ResConf, 1).
simulateAll(Conf, State, Pos, NewState, NewPos, [NewState, NewPos, ResConf], Len) :-
	simulateLenTS(Conf, State, Pos, Len, NewState, NewPos, ResConf).
simulateAll(Conf, State, Pos, NewState, NewPos, [SS,PP,X|ResConf], Len) :- 	
	simulateLenTS(Conf, State, Pos, Len, SS, PP, X), 
	Len1 is Len + 1, 
	simulateAll(Conf, State, Pos, NewState, NewPos, ResConf, Len1).

/**
 * Predikat pro vypis konfigurace na vystup.
 * printSingleConf(State, Pos, Conf)
 * @param State Aktualni stav.
 * @param Pos Aktualni pozice na pasce.
 * @param Conf Obsah pasky TS.
 */
printSingleConf(_, _, []) :- write('\n'), !.
printSingleConf(State, Pos, [X|Conf]) :- 
	(Pos = 0 -> (write(State),printSingleConf(State,-1, [X|Conf])); 
	(write(X),Pos1 is Pos - 1, printSingleConf(State,Pos1,Conf))).

/**
 * Predikat pro vypis vsech konfiguraci na vystup (konfigurace jsou ulozeny v poli).
 * printOutput(N, Conf)
 * @param N Pocet konfiguraci v poli, ktere se maji vypsat.
 * @param Conf Seznam konfiguraci TS.
 */
printOutput(0, []) :- !.
printOutput(_, []).
printOutput(N, [S,P,C|A]) :- 
	printSingleConf(S, P, C), 
	N1 is N - 1, 
	printOutput(N1, A).

/**
 * Predikat pro nacteni radku ze vstupu.
 * read_line(L, C)
 * @param L Seznam znaku na radku.
 * @param C Zpracovavany znak.
 */
read_line(L,C) :- 
	get_char(C), 
	(isEOFEOL(C), L = [], !; read_line(LL,_), atom_codes(C,[_]),[C|LL] = L ).

/**
 * Predikat pro kontrolu, zda je znak konec souboru.
 * isEOFEOL(C)
 * @param C Zpracovavany znak.
 */
isEOFEOL(C) :- 
	C == end_of_file; 
	(char_code(C,Code), Code==10).

/**
 * Predikat pro cteni radku ze vstupu.
 * read_lines(Ls)
 * @param Ls Seznam nactenych radku.
 */
read_lines(Ls) :- 
	read_line(L, C), 
	(C == end_of_file, Ls=[] ; 
	(read_lines(LLs), [L|LLs] = Ls)).

/**
 * Predikat pro pridani pravidla prechodu do databaze.
 * addRule(L)
 * @param L Seznam prechodu, ktere se maji pridat. Kazdy prechod je ctverice 
 * (Stav poc, cteny znak, Cilovy stav, Akce).
 */
addRule([]) :- !.
addRule([[From, _, Read, _, To, _, Act]|R]) :- 
	(Pred =.. [transition, From, Read, To, Act], 
	assertz(Pred)), addRule(R), !.

/**
 * Predikat pro zpracovani vstupu (pridani pravidel do databaze, nacteni obsahu pasky).
 * processInput(Tap)
 * @param Tap Nacteny obsah pasky.
 */
processInput(Tap) :- 
	retractall(transition(_,_,_,_)), 
	read_lines(Ls), 
	append(Init, [Tap], Ls),
	addRule(Init).

/**
 * Predikat pro simulaci nedeterministickeho Turingova stroje. Nejprve se hleda posloupnost 
 * konfiguraci do ciloveho stavu. Pokud takova posloupnost neexistuje je hledana posloupnost 
 * konfiguraci, ze kterych jiz nelze dal prejit. Nakonec je nalezena posloupnost konfiguraci 
 * vypsana na vystup.
 */
runTuringMachine :- 
	prompt(_, ''),
	processInput(Tap), !,
	((simulateAll(Tap, 'S', 0, 'F', P, Res), P >= 0) -> 
		true; 
		(((simulateAll(Tap, 'S', 0, A, B, [C, D, K|R]), \+(step(K, A, B, _, _, _)), [C, D, K|R] = Res)) -> 
			true; 
			Res = [])), 
	length(Res, S), 
	write('S'), printSingleConf('S', -1, Tap),
	printOutput(S, Res), !.

