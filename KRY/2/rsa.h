
/**
 * Projekt c. 2 do predmetu KRY -- Implementace a prolomeni RSA.
 * Autor: Vojtech Havlena, xhavle03
 * Datum 25.4.2016
 */

#include <iostream>
#include <string>
#include <cstddef>
#include <gmpxx.h>

/**
 * Datova struktura pro ulozeni hodnot rozsireneho klice.
 */
struct ExtendedKey
{
   mpz_class p;
   mpz_class q;
   mpz_class n;
   mpz_class e;
   mpz_class d;
};

/**
 * Typy parametru.
 */
enum ParamsTypes
{
   GEN,
   CIP,
   DEC,
   BREAK,
   ERR,
   MODERR
};

/**
 * Datova struktura, ktera reprezentuje parametry programu.
 */
struct Params
{
   ParamsTypes type;
   long moduleSize;
   mpz_class e;
   mpz_class n;
   mpz_class mess;
   mpz_class d;
};

/* Horni mez pro trivialni deleni. */
const long PRIMES = 1000000;

/**
 * Prototypy funkci.
 */

bool SolovayStrassen(mpz_class n, int iter);
int JacobiSymbol(mpz_class a, mpz_class n);
mpz_class GenerateRandomPrime(unsigned long bits);
ExtendedKey GenerateKeys(unsigned long bits);
mpz_class EuclideanAlgorithm(mpz_class a, mpz_class b);
mpz_class ComputeExponent(mpz_class phi);
mpz_class Inverse(mpz_class a, mpz_class b);
void PrintExtendedKey(ExtendedKey key);

mpz_class Cipher(mpz_class exponent, mpz_class modulus, mpz_class message);
mpz_class Decipher(mpz_class d, mpz_class n, mpz_class message);

bool TrivialDivision(mpz_class * res, mpz_class n, long to);
bool BrentMethod(mpz_class * res, mpz_class n);
mpz_class Factorize(mpz_class n);

Params ParseParams(int argc, char * argv[]);

/**
 * Implementace minima nad typy mpz_class.
 */
inline mpz_class minimum(mpz_class a, mpz_class b)
{
   if(a < b)
      return a;
   return b;
}
