
/**
 * Projekt c. 2 do predmetu KRY -- Implementace a prolomeni RSA.
 * Autor: Vojtech Havlena, xhavle03
 * Datum 25.4.2016
 */

#include "rsa.h"

using namespace std;

/* Globalni promenna pro generovani nahodnych cisel. */
gmp_randclass rndClass (gmp_randinit_default);

/**
 * Hlavni funkce programu.
 * @param argc Pocet argumentu programu.
 * @param argv Jednotlive argumenty.
 * @return Uspesnost behu.
 */
int main(int argc, char *argv[])
{
   rndClass.seed(time(NULL));

   Params par = ParseParams(argc, argv);
   ExtendedKey generatedKey;
   mpz_class res;

   if(par.type == ERR)
   {
      cerr << "Spatne parametry programu." << endl;
      return -1;
   }
   if(par.type == MODERR)
   {
      cerr << "Velikost modulu musi byt alespon 6." << endl;
      return -1;
   }

   switch(par.type)
   {
      case GEN:
         generatedKey = GenerateKeys(par.moduleSize);
         PrintExtendedKey(generatedKey);
         break;

      case CIP:
         res = Cipher(par.e, par.n, par.mess);
         cout << hex << showbase << res << endl;
         break;

      case DEC:
         res = Decipher(par.d, par.n, par.mess);
         cout << hex << showbase << res << endl;
         break;

      case BREAK:
         res = Factorize(par.n);
         cout << hex << showbase << res << endl;
         break;
   }

   return 0;
}

/**
 * Funkce, ktera provede faktorizaci verejneho modulu. Nejprve provede
 * trivialni deleni a potom Brentovu metodu.
 * @param n Verejny modul, ktery se ma faktorizovat.
 * @return Faktor n.
 */
mpz_class Factorize(mpz_class n)
{
   mpz_class res;
   if(TrivialDivision(&res, n, PRIMES))
      return res;
   BrentMethod(&res, n);

   return res;
}

/**
 * Funkce, pro testovani prvociselnosti (pouzita metoda Solovay--Strassen).
 * @param n Cislo, ktere se ma zkontrolovat na prvocislo.
 * @param iter Pocet iteraci.
 * @return True -- Je pravdepodobne prvocislo, False -- urcite neni.
 */
bool SolovayStrassen(mpz_class n, int iter)
{
   mpz_class n3 = n - 3;
   mpz_class res, exp;
   mpz_class jacobi = 0;

   for(int i = 0; i < iter; i++)
   {
      mpz_class a = rndClass.get_z_range(n3) + 2;
      jacobi = JacobiSymbol(a, n);
      if(jacobi == 0)
         return false;
      exp = (n - 1) / 2;
      mpz_powm(res.get_mpz_t(), a.get_mpz_t(), exp.get_mpz_t(), n.get_mpz_t());
      if(jacobi == -1)
         jacobi += n;
      if(res != jacobi)
         return false;
   }

   return true;
}

/**
 * Funkce, ktera zpocita Jacobiho symbol (a/n).
 * @param a Prvni operand.
 * @param n Druhy operand.
 * @return Jacobiho symbol (a/n).
 */
int JacobiSymbol(mpz_class a, mpz_class n)
{
   int ret = 1;
   mpz_class tmp;
   while(a != 0)
   {
      while(a % 2 == 0)
      {
         a = a/2;
         if(n % 8 == 3 || n % 8 == 5)
            ret = -1 * ret;
      }
      tmp = a;
      a = n;
      n = tmp;
      if(a % 4 == 3 && n % 4 == 3)
         ret = -1 * ret;
      a = a % n;
   }
   if(n == 1)
      return ret;
   return 0;
}

/**
 * Funkce, ktera vygeneruje nahodne prvocislo s danym poctem bitu.
 * @param bits Minimalni pocet bitu (muze byt vygenerovano cislo, ktere je vetsi).
 * @return Vygenerovane nahodne prvocislo.
 */
mpz_class GenerateRandomPrime(unsigned long bits)
{
   mpz_class randInt = rndClass.get_z_bits(bits);
   mpz_setbit(randInt.get_mpz_t(), bits - 1);
   mpz_setbit(randInt.get_mpz_t(), bits - 2);
   mpz_setbit(randInt.get_mpz_t(), 0);

   while(!SolovayStrassen(randInt, 100))
      randInt += 2;

   return randInt;
}

/**
 * Funkce, ktera vygeneruje parametry RSA (klice) s danou velikosti
 * verejneho modulu.
 * @param bits Velikost modulu v bitech.
 * @return Verejny a soukromy klic RSA.
 */
ExtendedKey GenerateKeys(unsigned long bits)
{
   unsigned long half;
   mpz_class phi;
   ExtendedKey ret;

   if(bits % 2 == 0)
      half = bits / 2;
   else
      half = bits / 2 + 1;

   do
   {
      ret.p = GenerateRandomPrime(half);
      ret.q = GenerateRandomPrime(bits - half);
      while(ret.p == ret.q)
      {
         ret.q = GenerateRandomPrime(bits - half);
      }
      ret.n = ret.p*ret.q;
   } while(mpz_sizeinbase(ret.n.get_mpz_t(), 2) != bits);

   phi = (ret.p - 1)*(ret.q - 1);
   ret.e = ComputeExponent(phi);
   ret.d = Inverse(ret.e, phi);

   return ret;
}

/**
 * Eukliduv algoritmus pro vypocet gcd.
 * @param a, b Hodnoty jejichz gcd se ma spocitat.
 * @param gcd(a,b)
 */
mpz_class EuclideanAlgorithm(mpz_class a, mpz_class b)
{
   mpz_class tmp;
   while(b != 0)
   {
      tmp = b;
      b = a % b;
      a = tmp;
   }

   return a;
}

/**
 * Funkce, ktera spocita verejny exponent. Nejprve je testovano, jestli
 * neni mozne vyuzit Fermatova cisla. Pokud ne je vygenerovo nahodne
 * cislo gcd(e, phi) = 1.
 * @param phi Eulerova funkce na n.
 * @return Verejny exponent e.
 */
mpz_class ComputeExponent(mpz_class phi)
{
   mpz_class e;
   /*mpz_class FermatPrimes[5] = {3, 5, 17, 257, 65537};

   for(int i = 0; i < 5; i++)
   {
      if(EuclideanAlgorithm(FermatPrimes[i], phi) == 1)
         return FermatPrimes[i];
   }*/

   do
   {
      e = rndClass.get_z_range(phi - 2) + 2;
   } while(EuclideanAlgorithm(e, phi) != 1);

   return e;
}

/**
 * Funkce, ktera spocita inverzni prvek v telese Z_n. Vypocet je
 * provaden rozsirenym Euklidovym algoritmem, ktery hleda Bezoutuv
 * koeficient.
 * @param a Prvek, jehoz inverzni prvek se ma spocitat.
 * @param b modulo.
 * @return a^{-1}
 */
mpz_class Inverse(mpz_class a, mpz_class b)
{
   mpz_class x = 0;
   mpz_class y = 0;
   mpz_class u = 1;
   mpz_class q, k, r;
   mpz_class modulus = b;

   if(b == 0)
      return 1;


   while(a != 0)
   {
      q = b / a;
      r = b % a;
      k = x - u*q;
      b = a;
      a = r;
      x = u;
      u = k;
   }

   mpz_mod(x.get_mpz_t(), x.get_mpz_t(), modulus.get_mpz_t());

   return x;
}

/**
 * Funkce, ktera zasifruje zadanou zpravu verejnym klicem.
 * @param e Verejny exponent.
 * @param n Verejny modulus.
 * @param message Zprava, ktera se ma zasifrovat.
 * @return Zasifrovana zprava.
 */
mpz_class Cipher(mpz_class e, mpz_class n, mpz_class message)
{
   mpz_class cipher;
   mpz_powm(cipher.get_mpz_t(), message.get_mpz_t(), e.get_mpz_t(),
      n.get_mpz_t());
   return cipher;
}

/**
 * Funkce, ktera desifruje zadanou zpravu soukromym klicem.
 * @param d Soukromy exponent.
 * @param n Verejny modulus.
 * @param message Zprava, ktera se ma desifrovat.
 * @return Desifrovana zprava.
 */
mpz_class Decipher(mpz_class d, mpz_class n, mpz_class message)
{
   mpz_class decipher;
   mpz_powm(decipher.get_mpz_t(), message.get_mpz_t(), d.get_mpz_t(),
      n.get_mpz_t());
   return decipher;
}

/**
 * Funkce, ktera vytisky hodnotu rozsireneho klice na stdout.
 * @param key Klic, ktery se ma vypsat na stdout.
 */
void PrintExtendedKey(ExtendedKey key)
{
   cout << hex << showbase << key.p << " " << key.q << " " << key.n
      << " " << key.e << " " << key.d << endl;
}

/**
 * Funkce, ktera provede trivialni (zkusme) deleni. Je vyuzivano Eratosthenovo
 * sito.
 * @param res Vysledek
 * @param n Cislo, ktere se ma faktorizovat.
 * @param to Horni omezeni pro zkouseni cisel.
 * @return True -- nalezen delitel, jinak False.
 */
bool TrivialDivision(mpz_class * res, mpz_class n, long to)
{
   bool * sito = new bool[to];
   for(long i = 0; i < to; i++)
      sito[i] = true;

   for(long i = 2; i < to; i++)
   {
      if(sito[i])
      {
         if(n % i == 0)
         {
            *res = i;
            return true;
         }
         for(long j = i*2; j < to; j = j + i)
         {
            sito[j] = false;
         }
      }
   }

   return false;
}

/**
 * Brentova metoda pro faktorizaci verejneho modulu.
 * @param res Vysledek faktorizace (faktor).
 * @param n Verejny modul, ktery se ma faktorizovat.
 * @return True -- podarilo se najit delitel, jinak False.
 */
bool BrentMethod(mpz_class * res, mpz_class n)
{
   mpz_class x0 = rndClass.get_z_range(n - 2) + 1;
   mpz_class c = rndClass.get_z_range(n - 2) + 1;
   mpz_class r = 1;
   mpz_class q = 1;
   mpz_class g = 1;
   mpz_class m = rndClass.get_z_range(n - 2) + 1;
   mpz_class y, i, k, ys, x, mn;

   y = x0;
   do
   {
      x = y;
      for(i = 0; i < r; i++)
         y = ((y*y) % n + c) % n;
      k = 0;
      do
      {
         ys = y;
         mn = minimum(m, r - k);
         for(i = 0; i < mn; i++)
         {
            y = ((y*y) % n + c) % n;
            q = (q * abs(x - y)) % n;
         }
         g = EuclideanAlgorithm(q, n);
         k += m;
      } while(k < r && g == 1);
      r *= 2;
   } while(g == 1);
   if(g == n)
   {
      do
      {
         ys = ((ys*ys) % n + c) % n;
         g = EuclideanAlgorithm(abs(x-ys), n);
      } while(g == 1);
   }
   *res = g;
   if(g == n)
      return false;
   return true;
}

/**
 * Funkce, ktera zpracuje parametry programu.
 * @param argc Pocet parametru programu.
 * @param argv Jednotlive parametry.
 */
Params ParseParams(int argc, char * argv[])
{
   Params ret;
   ret.type = ERR;
   string tmp;

   if(argc < 2)
   {
      return ret;
   }

   if(strcmp(argv[1], "-g") == 0)
   {
      if(argc != 3)
         return ret;
      tmp = string(argv[2]);

      ret.type = GEN;
      std::string::size_type sz;
      try
      {
         ret.moduleSize = std::stol(tmp, &sz);
      }
      catch(invalid_argument arg)
      {
         ret.type = ERR;
         return ret;
      }
      if(sz != tmp.size() || ret.moduleSize <= 0)
      {
         ret.type = ERR;
         return ret;
      }
      if(ret.moduleSize <= 6)
      {
         ret.type = MODERR;
         return ret;
      }

      return ret;
   }
   else if(strcmp(argv[1], "-e") == 0)
   {
      if(argc != 5)
         return ret;

      try
      {
         tmp = string(argv[2]);
         ret.e = mpz_class(tmp);
         tmp = string(argv[3]);
         ret.n = mpz_class(tmp);
         tmp = string(argv[4]);
         ret.mess = mpz_class(tmp);
         ret.type = CIP;
      }
      catch(invalid_argument arg)
      {
         ret.type = ERR;
         return ret;
      }

      return ret;
   }
   else if(strcmp(argv[1], "-d") == 0)
   {
      if(argc != 5)
         return ret;

      try
      {
         tmp = string(argv[2]);
         ret.d = mpz_class(tmp);
         tmp = string(argv[3]);
         ret.n = mpz_class(tmp);
         tmp = string(argv[4]);
         ret.mess = mpz_class(tmp);
         ret.type = DEC;
      }
      catch(invalid_argument arg)
      {
         ret.type = ERR;
         return ret;
      }

      return ret;
   }
   else if(strcmp(argv[1], "-b") == 0)
   {
      if(argc != 3)
         return ret;
      tmp = string(argv[2]);

      ret.type = BREAK;
      ret.n = mpz_class(tmp);

      return ret;
   }
   return ret;
}
