
-- Nazev projektu: RKA-2-DKA
-- Autor: Vojtech Havlena
-- Login: xhavle03
-- Datum: 3.3.2016

import System.Environment
import System.IO
import System.IO.Error
import System.Directory
import Automaton
import InputParser
import qualified Data.Map as Map

-- |Funkce, ktera podle parametru programu otevre odpovidajici Handle pro cteni.
-- In: Parametry programu -- seznam retezcu.
-- Out: Monada Handle
openStream :: [String] -> IO Handle
openStream args = do
   if (length args) == 0 then return (stdin)
   else openFile (args !! 0) ReadMode

-- |Funkce pouzivana pro zpracovani vyjimky pri otevirani souboru.
-- In: Vznikla chyba.
-- Out: IO().
catchBlock :: IOError -> IO ()
catchBlock error
   | isDoesNotExistError error = putStrLn "Chyba, neexistujici soubor."
   | otherwise = putStrLn "Chyba pri zpracovavani vstupu."  

-- |Funkce, ktera implementuje volbu programu -i (nacteni do vnitrni reprezentace a vypis).
-- In: Argumenty programu -- seznam retezcu.
-- Out: IO().
loadPrint :: [String] -> IO ()
loadPrint args = do
   handle <- openStream args
   whole <- hGetContents handle
   let parsedLines = lines whole
   if (length parsedLines) < 3 then 
      error "Spatny format vstupnich dat."
   else do
      let automat = parseAutomaton parsedLines
      if not (isAutomataValid automat) then
         error "Spatny format dat."
      else do
         putStr (show automat)
         hClose handle

-- |Funkce, ktera je volana v pripade spatnych parametru programu.
-- In: Argumenty programu -- seznam retezcu.
-- Out: IO().
printError :: [String] -> IO ()
printError args = do
   error "Chybne parametry programu."

-- |Funkce, ktera vraci asociativni pole, kde pro kazdou volbu programu existuje 
-- akce, ktera je vyvolana pri dane volbe.
-- Out: Asociativni pole dvojic (volba programu, akce).
switch :: Map.Map String ([String] -> IO ())
switch = Map.fromList [("-i", loadPrint), ("-t", convertToDka)]

-- |Funkce, ktera odpovida volbe -t (prevod RKA na DKA).
-- In: Argumenty programu -- seznam retezcu.
-- Out: IO().
convertToDka :: [String] -> IO ()
convertToDka args = do
   handle <- openStream args
   whole <- hGetContents handle
   let parsedLines = lines whole
   if (length parsedLines) < 3 then 
      error "Spatny format vstupnich dat."
   else do
      let automat = parseAutomaton parsedLines
      if not (isAutomataValid automat) then
         error "Spatny format dat."
      else do
         let convertedAut = rkaToDka automat
         putStr (show convertedAut)
         hClose handle
 
-- |Vstupni funkce celeho programu.
main :: IO ()
main = do
   args <- getArgs
   if (length args) <= 0 || (length args) > 2 then
      error "Chybne parametry programu."
   else do
      let choice = Map.findWithDefault (printError) (head args) switch
      catchIOError (choice (tail args)) catchBlock
