<?php

#JMP:xhavle03
/*
 * Soubor: Lex.php
 * Datum: 6. 3. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Lexikalni analyzator pro 1. projekt IPP.
 */

/* Typy tokenu. */
define("ERROR", -1);
define("CHAR", 0);
define("LBRACE", 1);
define("RBRACE", 2);
define("MACRO", 3);
define("IDENT", 4);
define("EOF", 5);
define("EOK", 6);

/* Typy chyb. */
define("EPARAM", 1);
define("EXFILE", 2);
define("FILEOPEN", 3);
define("EFILECOD", 4);
define("ESYN", 55);
define("ESEM", 56);
define("ERED", 57);

/* Jednotlive chybove hlasky. */
$errorMsg = array(EPARAM => "Chybne parametry prikazoveho radku", 
	EXFILE => "Neexistujici soubor nebo chyba pri otevirani", 
	FILEOPEN => "Chyba pri otevreni souboru pro zapis", 
	ESYN => "Syntakticka chyba", 
	ESEM => "Semanticka chyba",
	ERED => "Chyba, redefinice makra",
	EFILECOD => "Chybne kodovani souboru");

/* Trida simulijici cinnost lexikalniho analyzatoru. */
class Lexical
{
   /* Promenna pro typ tokenu. */
   public $token;
   /* Promenna pro data spojena s tokenem. */
   public $data;
   /* Stav KA a escape znaky. */
   private $state;
   private $escape = array('@', '{', '}', '$');
   /* Historie znaku pro funkci UngetChar. */
   private $history;
   private $fileDesc;
   private $inputEmpty;
   private $len;
   
   function __construct($fileInput)
   {
      $this->state = 0;
      $this->token = -1;
      $this->data = "";
      $this->history = "";
      $this->fileDesc = $fileInput;
      $this->len = 0;
      $this->inputEmpty = false;
   }
   
   /* 
    * Funkce, ktera vraci znak ze vstupu. 
    */
   private function GetChar()
   {
      if($this->len == 0 && feof($this->fileDesc))
         $this->inputEmpty = true;
      
      if($this->len > 0)
      {
         $tmp = $this->history[0];
         $this->history = substr($this->history, 1);
         $this->len--;
         return $tmp;
      }
      return fread($this->fileDesc, 1);
   }
   
   /*
    * Funkce, ktera prida do historie zadany retezec.
    * @param $string Retezec, ktery se ma pridat do fronty.
    */
   public function AddHistory($string)
   {
      $this->inputEmpty = false;
      $this->history = $string.$this->history;   
      $this->len += strlen($string);
   }
   
   /* 
    * Funkce, ktera vraci znak do vstupniho bufferu.
    * @param $char Znak, ktery se ma vratit. 
    */
   private function UngetChar($char)
   {
      if($this->inputEmpty)
         return;
      if($char)
      {
         $this->history = $char.$this->history;
         $this->len++;
      }
   }
   
   /* 
    * Funkce, ktera vraci nacteny a zpracovany token. 
    */
   function GetToken()
   {
      $this->token = EOF;
      $this->data = "";
      $this->state = 0;
      while(true)
      {
         $char = $this->GetChar();
         switch($this->state)
         {
         case 0:
            if($char == '@') 
            {
               $this->token = MACRO;
               $this->state = 1;
            }
            else if($char == '{')
            {
               $this->token = LBRACE;
               $this->data = $char;
               return;
            }
            else if($char == '}')
            {
               $this->token = RBRACE;
               $this->data = $char;
               return;
            }
            else if($char == '$')
            {
               $this->state = 3;
               $this->token = IDENT;
            }
            else if($this->inputEmpty)
            {
               $this->token = EOF;
               return;
            }
            else
            {
                $this->token = CHAR;
                $this->data = $char;
                return;
            }
            
            break;
            
         case 1:
            if(($char >= 'a' && $char <= 'z') || ($char >= 'A' && 
               $char <= 'Z') || $char == '_')
            {
               $this->state = 2;
               
               $this->data = "@";
               $this->data .= $char;
            }
            else if(in_array($char, $this->escape))
            {
               $this->token = CHAR;
               $this->data = $char;
               return;
            }
            else
            {
               $this->token = ERROR;
               $this->data = ESYN;
               return;
            }
            break;
            
         case 2:
            if(($char >= 'a' && $char <= 'z') || ($char >= 'A' && 
               $char <= 'Z') || $char == '_' || ($char >= '0' && $char <= '9'))
            {
               $this->data .= $char;
            }
            else
            {
               $this->UngetChar($char);
               return;
            }
            break;
            
         case 3:
            if(($char >= 'a' && $char <= 'z') || ($char >= 'A' && 
               $char <= 'Z') || $char == '_')
            {
               $this->state = 2;
               
               $this->data = "$";
               $this->data .= $char;
            }
            else
            {
               $this->token = ERROR;
               $this->data = ESYN;
               return;
            }
            break;
         }
      }
   }
   
   /*
    * Funkce, ktera nacte token v bloku.
    */
   function GetBlockToken($varMode)
   {
      $this->state = 0;
      $this->token = EOF;
      while($char = $this->GetChar())
      {
         switch($this->state)
         {
         case 0:
            if($char == '@') 
            {
               $this->token = CHAR;
               $this->data = $char;
               $this->state = 1;
            }
            else if($char == '{')
            {
               $this->token = LBRACE;
               $this->data = $char;
               return;
            }
            else if($char == '}')
            {
               $this->token = RBRACE;
               $this->data = $char;
               return;
            }
            else if($char == '$' && $varMode)
            {
               $this->state = 3;
               $this->token = IDENT;
            }
            else if($this->inputEmpty)
            {
               $this->token = EOF;
               return;
            }
            else
            {
                $this->token = CHAR;
                $this->data = $char;
                return;
            }
            
            break;
            
         case 1:
            if($char == '$' && $varMode)
            {
               $this->token = CHAR;
               $this->data = $char;
               return;
            }
            else if($char != '$' && in_array($char, $this->escape) && !$varMode)
            {
               $this->token = CHAR;
               $this->data = $char;
               return;
            }
            else
            {
               $this->UngetChar($char);
               $this->token = CHAR;
               $this->data = '@';
               return;
            }
            break;
            
         case 2:
            if(($char >= 'a' && $char <= 'z') || ($char >= 'A' && 
               $char <= 'Z') || $char == '_' || ($char >= '0' && $char <= '9'))
            {
               $this->data .= $char;
            }
            else
            {
               $this->UngetChar($char);
               return;
            }
            break;
            
         case 3:
            if(($char >= 'a' && $char <= 'z') || ($char >= 'A' && 
               $char <= 'Z') || $char == '_')
            {
               $this->state = 2;
               
               $this->data = "$";
               $this->data .= $char;
            }
            else
            {
               $this->token = CHAR;
               $this->UngetChar($char);
               return;
            }
            break;
         }
      }
   }
}

?>
