<?php

#JMP:xhavle03
/*
 * Soubor: Semantic.php
 * Datum: 6. 3. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Semanticka a syntakticka kontrola spravnosti programu. 
 */

/* Trida implementujici tabulku symbolu. */
class TableItem
{
   public $paramsCount; /* Pocet parametru. */
   public $body; /* Telo makra (pole). */
   public $paramsList; /* Pole parametru. */
   public $paramsPlace;
   
   public $buildIn; /* Zde je zapsan nazev buildin makra, ktere 
      je s polozkou ekvivalentni.  */
   
   public function __construct()
   {
      $this->paramsCount = 0;
      $this->body = array();
      $this->paramsList = array();
      $this->buildIn = "";
   }
}

/* 
 * Funkce, ktara zpracuje makro @let.
 * @param $lex Trida implementujici lex. analyzator
 * @param $table Tabulka symbolu.
 * @param $redifMacro Seznam maker, ktere neni mozne redefinovat.
 * @param $skipSpaces Maji se ignorovat mezery?
 */
function macroLet(&$lex, &$table, $redifMacro, $skipSpaces, $redefine)
{
   GetTokenOpt($lex, $skipSpaces); 
   if($lex->token == ERROR) return $lex->data;
   if($lex->token != MACRO)
      return ESEM;
   $name1 = $lex->data;
   if($name1 == "@null")
      return EOK;
   if(in_array($name1, $redifMacro))
      return ERED;
   
   GetTokenOpt($lex, $skipSpaces);
   if($lex->token == ERROR) return $lex->data;
   if($lex->token != MACRO)
      return ESEM;
   $name2 = $lex->data;
   if(!array_key_exists($name2, $table))
      return ESEM;
   if(!$redefine && $name2 != "@null" && array_key_exists($name1, $table))
      return ERED;
   if($name2 == "@null")
   {
      unset($table[$name1]); /* Zrusime hodnotu v TS. */
   }
   else
   {
      /* Zkopirujeme funkcionalitu. */
      $table[$name1] = $table[$name2];
   }
      
   return EOK;
}

/*
 * Funkce, ktera nacte dalsi token, pripadne ignoruje bile znaky.
 * @param $lex Lexikalni analyzator.
 * @param $skip Maji se ignorovat bile znaky?
 */
function GetTokenOpt(&$lex, $skip)
{
   $lex->GetToken();
   if($skip) skipSpaces($lex);
}

/*
 * Funkce, ktera zpracuje aktualni makro.
 * @param $lex Lexikalni analyzator.
 * @param $table Tabulka symbolu.
 * @param $redfiMacro Pole maker, ktere nemuzou byt redefinovana.
 * @param $skipSpace Maji se ignorovat bile znaky?
 * @param $fileOutput Vystupni stream.
 * @param $redefine Mohou se redefinovat makra?
 */
function processMacro(&$lex, &$table, $redfiMacro, 
   &$skipSpaces, $fileOutput, $redefine)
{
   /* Makro neexistuje v TS. */
   if(!array_key_exists($lex->data, $table))
      return ESEM;
   
   if($table[$lex->data]->buildIn == "@def")
   {
      GetTokenOpt($lex, $skipSpaces);  
      if($lex->token == ERROR) return $lex->data;
      return macroDefinition($lex, $table, $redfiMacro, 
         $skipSpaces, $fileOutput, $redefine);
   }
   else if($lex->data == "@null")
   {
	   return EOK;
   }
   else if($table[$lex->data]->buildIn == "@let")
   {
	   return macroLet($lex, $table, $redfiMacro, $skipSpaces, $redefine);
   }
   else if($table[$lex->data]->buildIn == "@set")
   {
      return macroSet($lex, $skipSpaces, $fileOutput);
   }
   else /* Provedeme expanzi makra. */
   {
      return macroExpansion($lex, $table, $fileOutput, $skipSpaces);
   }
   
   return EOK;
}

/*
 * Funkce, ktera provede expanzi aktualniho makra.
 * @param $lex Lexikalni analyzator.
 * @param $table Tabulka symbolu.
 * @param $skipSpace Maji se ignorovat bile znaky?
 * @param $fileOutput Vystupni stream.
 */
function macroExpansion(&$lex, &$table, $fileOutput, &$skipSpaces)
{
   $name = $lex->data;
   $params = array();
   $tmp = "";
   $index = 0;

   /* Provedeme tolik nacteni, kolik mame parametru. */
   for($i = 0; $i < $table[$name]->paramsCount; $i++)
   {
      GetTokenOpt($lex, $skipSpaces); 
      if($lex->token == ERROR) return ESYN;
      if($lex->token == EOF) return ESEM;
      
      if($lex->token == LBRACE)
      {
         $tmp = "";
         processBlock($lex, $fileOutput, $tmp);
         $params[$i] = $tmp;
      }
      else
         $params[$i] = $lex->data;
   }
   
   $res = "";
   for($i = 0; $i < strlen($table[$name]->body); $i++)
   {
      if(isset($table[$name]->paramsPlace[$i]))
      {
         $key = array_search($table[$name]->paramsPlace[$i], 
            $table[$name]->paramsList);
         $res = $res.$params[$key];
         continue;
      }
      $res = $res.$table[$name]->body[$i];
   }
   $lex->AddHistory($res);

   return EOK;
}

/*
 * Funkce, ktera preskoci vsechny bile znaky ze vstupu.
 * @param $lex Lexikalni analyzator.
 */
function skipSpaces(&$lex)
{
   while($lex->token == CHAR)
   {
      if(!ctype_space($lex->data))
         return;
      $lex->GetToken(); 
   }
}

/*
 * Funkce, ktera zpracuje blok ohraniceny znaky { }.
 * @param $lex Lexikalni analyzator.
 * @param $fileOutput Vystupni soubor.
 * @param $checkIdent Ma se kontrolovat na pritomnost identifikatoru.
 * @param $dest Retezec pro ulozeni obsahu bloku (musi byt prazdny).
 * @param $span Ma se retezec rozdelit logicky do pole?
 */
function processBlock(&$lex, $fileOutput, &$dest = null)
{
   $count = 1;
   if($dest != null) $dest = "";
   while($count != 0)
   {
      $lex->GetBlockToken(false); //if($lex->token == ERROR) return $lex->data;
      if($lex->token == LBRACE)
         $count++;
      else if($lex->token == RBRACE)
         $count--;
      else if($lex->token == EOF)
         return ESYN;
         
      if($count < 0) return ESYN;
      if($count == 0) break; //?????????????????
       
      if($dest === NULL)
         fwrite($fileOutput, $lex->data);
      else
         $dest .= $lex->data;
   }
   
   return EOK; 
}

/*
 * Funkce, ktera provede definici noveho makra.
 * @param $lex Lexikalni analyzator.
 * @param $table Tabulka symbolu.
 * @param $redfiMacro Pole maker, ktere nemuzou byt redefinovana.
 * @param $skipSpace Maji se ignorovat bile znaky?
 * @param $fileOutput Vystupni stream.
 * @param $redefine Mohou se redefinovat makra?
 */
function macroDefinition(&$lex, &$table, $redifMacro, $skipSpaces, 
   $fileOutput, $redefine)
{
   if($lex->token != MACRO)
      return ESEM;
   
   $name = $lex->data;
   if(in_array($name, $redifMacro))
      return ERED;
   if(!$redefine && array_key_exists($lex->data, $table) && $name != "@null")
      return ERED;
  
   GetTokenOpt($lex, $skipSpaces); 
   if($lex->token == ERROR) return $lex->data;
   if($lex->token != LBRACE)
      return ESEM;
   
   GetTokenOpt($lex, $skipSpaces); 
   if($lex->token == ERROR) return $lex->data;
   $table[$name] = new TableItem();
   /* Nacteni parametru. */
   while($lex->token != RBRACE)
   {
      skipSpaces($lex);
      if($lex->token == RBRACE)
         break;
      if($lex->token != IDENT)
         return ESYN;
      $table[$name]->paramsCount++;
      if(in_array($lex->data, $table[$name]->paramsList))
         return ESEM;
      array_push($table[$name]->paramsList, $lex->data); 
      $lex->GetToken();
      if($lex->token == ERROR) return $lex->data;
   }
   
   /* Nacteni tela. */
   GetTokenOpt($lex, $skipSpaces); 
   if($lex->token == ERROR) return $lex->data;
   if($lex->token != LBRACE)
      return ESEM; 

   $count = 1;
   $table[$name]->body = "";
   while($count != 0)
   {
      if($count < 0) return ESYN;
      
      $lex->GetBlockToken(true); if($lex->token == ERROR) return $lex->data;
      if($lex->token == LBRACE)
         $count++;
      else if($lex->token == RBRACE)
         $count--;
      else if($lex->token == EOF)
         return ESYN;
      else if($lex->token == IDENT && in_array($lex->data, 
         $table[$name]->paramsList))
      {
         $table[$name]->paramsPlace[strlen($table[$name]->body)] =  
             $lex->data;
         $lex->data = '-';
      }

      if($count == 0) break;
      $table[$name]->body .= $lex->data;
   }
   return EOK;
}

/*
 * Funkce, ktera provede semantiku makra set.
 * @param $lex Lexikalni analyzator.
 * @param $skipSpace Maji se ignorovat bile znaky?
 * @param $fileOutput Vystupni stream.
 */
function macroSet(&$lex, &$skipSpaces, $fileOutput)
{
   GetTokenOpt($lex, $skipSpaces);
   if($lex->token != LBRACE)
      return ESEM;
   
   $str = "";
   $index = 0;
   if(!processBlock($lex, $fileOutput, $str))
      return ESYN;
   if($str == "-INPUT_SPACES")
      $skipSpaces = true;
   else if($str == "+INPUT_SPACES")
      $skipSpaces = false;
   else
      return ESEM;
   
   return EOK;
}

?>
