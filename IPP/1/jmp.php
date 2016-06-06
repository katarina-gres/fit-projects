<?php

#JMP:xhavle03
/*
 * Soubor: jmp.php
 * Datum: 6. 3. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Reseni projektu JMP pro predmet IPP.
 */

include_once("lex.php");
include_once("semantic.php");

/* Nacteni parametru programu. */
$long = array("help", "input:", "output:", "cmd:");
$options = getopt("r", $long);
if(count($options) != count($argv) - 1)
{
   printError(EPARAM);
   exit(EPARAM);
}

$fileInput = STDIN;
$fileOutput = STDOUT;
$cmd = "";
$redefine = true;
/* Zpracovani jednotlivych parametru. */
foreach($options as $key => $value)
{
   if($key == "input")
   {
      if(!is_readable($value)) /* Soubor neexistuje nebo neni pro cteni. */
      {
         printError(EXFILE);
         closeFiles();
         exit(EXFILE);
      }
      $fileInput = fopen($value, "r");
      if(!$fileInput)
      {
         printError(EXFILE);
         closeFiles();
         exit(EPARAM);
      }
      if(mb_detect_encoding(file_get_contents($value), 'ASCII', true) === false)
      {
         printError(EFILECOD);
         closeFiles();
         exit(EFILECOD);
      }
   }
   else if($key == "output")
   {
      $fileOutput = @fopen($value, "w");
      if(!$fileOutput)
      {
         printError(FILEOPEN);
         closeFiles();
         exit(FILEOPEN);
      }
   }
   else if($key == "cmd")
   {
      $cmd = $value;
   }
   else if($key == "help" && count($argv) == 2)
   {
      echo "Uloha JMP (jednoduchy makroprocesor), Vojtech Havlena\n";
      echo "--help Zobrazeni napovedy\n";
      echo "--input=filenam Vstup bude nacitan ze souboru filename\n";
      echo "--output=filename Vystup bude ulozen do souboru filename\n";
      echo "--cmd=text text bude před začátkem zpracování vstupu vložen na začátek vstupu\n";
      echo "-r redefinice již definovaného makra pomocí makra @def způsobí chybu\n";
      exit(0);
   }
   else if($key == "r")
   {
      $redefine = false;
   }
   else
   {
      printError(EPARAM);
      closeFiles();
      exit(EPARAM);
   }
}

/* Vypiseme hodnotu parametru cmd. */
$lex = new Lexical($fileInput);
$lex->AddHistory($cmd);
$skipSpaces = false;
$table = array("@def" => new TableItem(), "@let" => new TableItem(), 
   "@set" => new TableItem(), "@null" => new TableItem(), 
   "@__def__" => new TableItem(), "@__let__" => new TableItem(), 
   "@__set__" => new TableItem());
$table["@def"]->buildIn = "@def";
$table["@let"]->buildIn = "@let";
$table["@set"]->buildIn = "@set";
$table["@__def__"]->buildIn = "@def";
$table["@__let__"]->buildIn = "@let";
$table["@__set__"]->buildIn = "@set";
$redfiMacro = array("@__def__", "@__let__", "@__set__");
$index = 0;
$celek = "";
/* Postupne nacitani vstupu. */
while($lex->token != EOF)
{
   $lex->GetToken();
   if($lex->token == CHAR)
   {
      if(!$skipSpaces || !ctype_space($lex->data))
         fwrite($fileOutput, $lex->data);
   }
   else if($lex->token == MACRO)
   {
      if(($res = processMacro($lex, $table, $redfiMacro, $skipSpaces, 
         $fileOutput, $redefine)) != EOK)
      {
         printError($res);
         closeFiles();
         exit($res);
      }
   }
   else if($lex->token == LBRACE)
   {
      if(($res = processBlock($lex, $fileOutput)) != EOK)
      {
         printError($res);
         closeFiles();
         exit($res);
      }
   }
   else if($lex->token == ERROR)
   {
      printError($lex->data);
      closeFiles();
      exit($lex->data);
   }
   else if($lex->token != EOF)
   {
      printError(ESYN);
      closeFiles();
      exit(ESYN);
   }
}

/*
 * Funkce, ktera vytiskne napovedu podle zadaneho identifikatoru chyby.
 * @param $code ID chyby.
 */
function printError($code)
{
   global $errorMsg;
   fwrite(STDERR, $errorMsg[$code] . "\n");
}

/*
 * Funkce, ktera uzavre soubory pouzivane v programu.
 */
function closeFiles()
{
   global $fileInput,$fileOutput;
   if($fileInput != STDIN && $fileInput)
   {
      fflush($fileInput);
      fclose($fileInput);
   }
   if($fileOutput != STDOUT && $fileOutput)
   {
      fflush($fileOutput);
      fclose($fileOutput);
   }
}

?>
