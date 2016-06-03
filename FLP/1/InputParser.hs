
-- Nazev projektu: RKA-2-DKA
-- Autor: Vojtech Havlena
-- Login: xhavle03
-- Datum: 3.3.2016

module InputParser
(parseStates,
 parseInitial,
 parseFinals,
 parseTransitions,
 parseAutomaton,
 isAutomataValid
) where

import qualified Data.Map as Map
import qualified Data.Set as Set
import qualified Data.List as List
import qualified Data.Char as Char;
import System.IO
import Automaton

-- |Funkce, ktera ze vstupniho retezce naparsuje mnozinu stavu.
-- In: Retezec, ktery reprezentuje stavy.
-- Out: Mnozina nactenych stavu.
parseStates :: String -> DKAStates
parseStates line = Set.fromList intStates 
   where intStates = map (\a -> if all Char.isDigit a then read a :: Int 
								        else error "Spatny format vstupnich dat.") strStates
         strStates = filter (any (/=',')) groupedStates
         groupedStates = List.groupBy (\a b -> (a == ',') == (b == ',')) line

-- |Funkce, ktera ze vstupniho retezce naparsuje pocatecni stav.
-- In: Retezec, ktery reprezentuje pocatecni stav.
-- Out: Naparsovany pocatecni stav.
parseInitial :: String -> Int
parseInitial line = if (all Char.isDigit line) && (length line) > 0 then read line :: Int
					     else error "Spatny format vstupnich dat."

-- |Funkce, ktera ze vstupniho retezce naparsuje mnozinu koncovych stavu.
-- In: Retezec, ktery reprezentuje mnozinu koncovych stavu.
-- Out: Naparsovana mnozina koncovych stavu.
parseFinals :: String -> Set.Set Int
parseFinals = parseStates

-- |Funkce, ktera ze seznamu retezcu, ktere reprezentuji prechodovou funkci RKA.
-- In: Seznam retezcu, ktery reprezentuje prechodu vstupniho RKA.
-- Out: Naparsovana mnozina prechodu.
parseTransitions :: [String] -> Transition
parseTransitions [] = Map.empty
parseTransitions (x:xs) = Map.insertWith (Set.union) key (Set.fromList [value]) (parseTransitions xs)
   where key = if (length (filter (==',') x)) == 2 then g transition
               else error "Spatny format vstupnich dat."
         value = if all Char.isDigit (last transition) then read (last transition) :: Int
				     else error "Spatny format vstupnich dat."
         g [a,_] = (if all Char.isDigit a then read a :: Int
                    else error "Spatny format vstupnich dat.", '-')
         g [a,b,_] = (if all Char.isDigit a then read a :: Int
                      else error "Spatny format vstupnich dat.",
                      if (length b == 1) && (Char.isLower (head b)) then head b
                      else error "Spatny format vstupnich dat.")
         g _ = error "Spatny format vstupnich dat."
         transition = filter (any (/=',')) groupedStates
         groupedStates = List.groupBy (\a b -> (a == ',') == (b == ',')) x

-- |Funkce, ktera kontroluje, zda v jednotlivych prechodech jsou pouzity legalni stavy 
-- (tj. ty co jsou definovany v mnozine stavu).
-- In: Mnozina nactenych stavu RKA a prechodova funkce.
-- Out: True pokud jsou pouzity legalni stavy, jinak False.
validTransitions :: DKAStates -> Transition -> Bool
validTransitions sts trans = (boolList)
   where boolList = (Map.foldWithKey vals True trans)
         vals (st,_) s prev = (Set.member st sts) && (Set.isSubsetOf s sts) && prev

-- |Funkce, ktera kontroluje, zda je vstupni automat korektne definovan.
-- In: Nacteny vstupni RKA.
-- Out: True pokud je korektne definovan, jinak False.
isAutomataValid :: Automaton -> Bool
isAutomataValid (Automaton st _ tr init fin) = (Set.isSubsetOf fin st) && (Set.member init st) 
      && (validTransitions st tr)

-- |Funkce, ktera ridi cely postup parsovani automatu ze seznamu retezcu.
-- In: Seznam retezcu ve vstupni reprezentaci, ktere reprezentuji RKA.
-- Out: Vytvorena struktura automatu, ktery odpovida seznamu vstupnich retezcu.
parseAutomaton :: [String] -> Automaton
parseAutomaton lines = Automaton {
   states = parseStates (lines !! 0),
   alphabet = Set.fromList ['a'..'z'],
   transitions = parseTransitions (filter (/="") (drop 3 lines)),
   initial = parseInitial (lines !! 1),
   finals = parseFinals (lines !! 2)}


