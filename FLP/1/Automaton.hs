
-- Nazev projektu: RKA-2-DKA
-- Autor: Vojtech Havlena
-- Login: xhavle03
-- Datum: 3.3.2016

module Automaton 
(Automaton(..),
 rkaToDka,
 DKAStates,
 Transition
) where

import qualified Data.Map as Map
import qualified Data.Set as Set
import qualified Data.List as List
import qualified Data.Char as Char
import qualified Data.Maybe as May

-- |Reprezentace prechodove funkce \delta. Predstavuje zobrazeni z dvojice 
-- (stav, symbol) do mnoziny stavu.
type Transition = Map.Map (Int, Char) (Set.Set Int)
-- |Interni reprezentace stavu DKA, ktery je preveden z NKA (kazdy stav 
-- DKA odpovida mnozine stavu NKA).
type DKAStatesSet = Set.Set (Set.Set Int)
-- |Typ reprezentujici mnozinu stavu.
type DKAStates = Set.Set Int
-- |Typ slouzici pro prejmenovani mnoziny stavu na jediny stav (cislo).
type RenameMap = Map.Map (Set.Set Int) Int

-- |Struktura obecneho (NKA) konecneho automatu.
data Automaton = Automaton {
   states :: DKAStates,
   alphabet :: Set.Set Char,
   transitions :: Transition,
   initial :: Int,
   finals :: DKAStates
}

-- |Uprava vypisu automatu (volba -i)
instance Show Automaton where
   show (Automaton st _ tr init fin) = showStates (Set.toList st) ++ "\n" ++ show init ++ "\n" ++ showStates (Set.toList fin) ++ "\n" ++ showTransitions (Map.toList tr)
      where showStates [] = ""
            showStates [x] = (show x)
            showStates (x:xs) = (show x) ++ "," ++ showStates xs
            showTransitions [] = ""
            showTransitions ((f,s):xs) = showTransTriplet (cartProduct [f] (Set.toList s)) ++ (showTransitions xs)
            cartProduct a b = [(x,y) | x <- a, y <- b]
            showTransTriplet [] = ""
            showTransTriplet ((a,b):xs) = show (fst a) ++ "," ++ (showEpsilon (snd a)) ++ "," ++ show b ++ "\n" ++ (showTransTriplet xs)
               where showEpsilon a = if a == '-' then [] else [a]

-- |Funkce, ktera na zaklade elementu a asociativniho pole, ve kterem kazdemu 
-- elementu je prirazeno cislo, vraci prave toto cislo (jednoznacny index). 
-- V pripade, kdyz neni hodnota v asoc. poli nalezena, je vracena hodnota (-1).
-- In: element, jehoz index se ma zjistit (mnozina Int) a asociativni pole prvku a indexu.
-- Out: Index prvku
makeIndex :: DKAStates -> RenameMap -> Int
makeIndex elem renameMap = Map.findWithDefault (-1) elem renameMap

-- |Funkce, ktera z daneho seznamu udela seznam dvojic, kde prvni slozka 
-- je puvodni hodnota a druha slozka je jeho index v seznamu.
-- In: Seznam prvku, ke kterym se maji priradit indexy, Pocatecni index
-- Out: Seznam dvojic (index, prvek)
renameList :: [a] -> Int -> [(a,Int)]
renameList [] _ = []
renameList (x:xs) i = (x,i) : renameList xs (i + 1)

-- |Funkce, ktera provede prechod automatu (pointwise extension funkce \delta), tj. \delta(Q, c).
-- In: Automat, symbol a seznam stavu, ze kterych se ma provest prechod
-- Out: Mnozina stavu do kterych vede prechod z Q po precteni symbolu c.
step :: Automaton -> Char -> [Int] -> DKAStates
step aut sym [] = Set.empty
step aut sym (x:xs) = Set.union (Map.findWithDefault (Set.empty) (x,sym) (transitions aut)) (step aut sym xs)

-- |Funkce, ktera vypocte epsilon uzaver mnoziny stavu.
-- In: Automat, mnozina stavu, jejich eps-cl se ma spocitat.
-- Out: Epsilon uzaver.
epsClosure :: Automaton -> DKAStates -> DKAStates
epsClosure aut q = if nextSet == q then nextSet
                   else epsClosure aut nextSet
   where singleEpsStep state = step aut '-' [state]
         nextSet = Set.union (Set.foldl (Set.union) Set.empty (Set.map (singleEpsStep) q)) q

-- |Funkce, ktera na zaklade pocatecniho stavu DKA automatu nalezne celou mnozinu stavu DKA 
-- (jsou uvazovany jen ty stavy, ktere jsou dostupne z poc. stavu).
-- In: RK Automat, mnozina zatim nalezenych stavu (stav DKA je mnozina stavu RKA).
-- Out: Mnozina stavu DKA = Mnozina mnozin stavu RKA.
dkaStates :: Automaton -> DKAStatesSet -> DKAStatesSet
dkaStates aut dkaSet = if newSet == dkaSet then dkaSet
                          else dkaStates aut newSet
   where newSet = Set.union (Set.fold (Set.union) Set.empty (Set.map (processState) dkaSet)) dkaSet
         processState dkaQ = Set.filter (/= Set.empty) (Set.fromList [ epsClosure aut (step aut s (Set.toList dkaQ)) | s <- ['a'..'z'] ])
   
-- |Funkce, ktera pro dany RKA spocita mnozinu stavu DKA (uvazuji se pouze dosazitelne stavy z pocatecniho stavu)
-- In: RK automat.
-- Out: Mnozina stavu DKA (stav je v DKA mnozina stavu RKA).
rkaToDkaStates :: Automaton -> DKAStatesSet
rkaToDkaStates aut = dkaStates aut (Set.fromList [epsClosure aut (Set.singleton (initial aut))])

-- |Funkce, ktera prejmenuje stavy v DKA (aby byly ve forme cisel ne mnoziny cisel).
-- In: Mnozina neprejmenovanych stavu DKA, asociativny pole, ktere obsahuje dvojice (stav DKA, index) 
-- a podle ni se provede prejmenovani.
-- Out: Nova mnozina stavu DKA.
rkaToDkaStatesRenamed :: DKAStatesSet -> RenameMap -> DKAStates
rkaToDkaStatesRenamed dkaStates renameMap = Set.map (rnm) dkaStates
   where newStatesList = Set.toList dkaStates
         rnm a = Map.findWithDefault (-1) a renameMap

-- |Funkce, ktera pro dany automat a stavy prevadeneho DKA (stav je mnozina stavu puvodniho automatu) zkonstruuje 
-- mnozinu prechodu DKA. Stavy jsou v mnozine prechodu prejmenovany pomoci asociativniho pole, kde je pro kazdou 
-- mnozinu odpovidajici ciselny stav.
-- In: Automat, mnozina neprejmenovanych stavu prevedeneho DKA, asoc. pole pro prejmenovani stavu.
-- Out: Mnozina prechodu DKA i s prejmenovanymi stavy.
rkaToDkaTransitions :: Automaton -> DKAStatesSet -> RenameMap -> Transition
rkaToDkaTransitions aut dkaStates renameMap = Map.fromList (Set.toList (Set.fold (Set.union) Set.empty (Set.map (makeTransitions) dkaStates)))
   where singleTrans dkaQ s = if nextState == Set.empty then 
                                 ((-1, '-'), Set.empty) 
                              else 
                                 (((makeIndex dkaQ renameMap), s), Set.singleton (makeIndex nextState renameMap))
            where nextState = epsClosure aut (step aut s (Set.toList dkaQ))
         makeTransitions dkaQ = Set.filter (/= ((-1,'-'),Set.empty)) (Set.fromList [ singleTrans dkaQ s | s <- ['a'..'z'] ])
      
-- |Funkce, ktera pro RKA zjisti novy pocatecni stav DKA.
-- In: RK automat, Asoc. pole pro prejmenovani stavu.
-- Out: Pocatecni stav v novem DKA.
rkaToDkaInitial :: Automaton -> RenameMap -> Int
rkaToDkaInitial aut renameMap = makeIndex (epsClosure aut (Set.singleton (initial aut))) renameMap

-- |Funkce, ktera pro RKA zjisti mnozinu novych koncovych stavu DKA.
-- In: RK automat, mnozina novych stavu DKA, Asoc. pole pro prejmenovani stavu.
-- Out: Mnozina koncovych stavu v novem DKA.
rkaToDkaFinals :: Automaton -> [DKAStates] -> RenameMap -> DKAStates
rkaToDkaFinals aut dkaStates renameMap = Set.fromList [makeIndex s renameMap | s <- dkaStates, ((Set.intersection (finals aut) s) /= Set.empty)]

-- |Funkce, ktera na zaklade drive definovanych funkci prevede RKA na DKA.
-- In: RKA
-- Out: DKA
rkaToDka :: Automaton -> Automaton
rkaToDka aut = Automaton {
   states = rkaToDkaStatesRenamed sts renameMap,
   alphabet = alphabet aut,
   transitions = rkaToDkaTransitions aut sts renameMap,
   initial = rkaToDkaInitial aut renameMap,
   finals = rkaToDkaFinals aut stsList renameMap 
   }
   where sts = rkaToDkaStates aut
         stsList = Set.toList sts
         renameMap = Map.fromList (renameList stsList 1)
         
