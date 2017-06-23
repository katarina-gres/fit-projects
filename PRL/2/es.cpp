/*
 * Projekt do predmetu PRL: Enumeration sort
 * Autor: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 *
 */

#include <mpi.h>
#include <iostream>
#include <fstream>

using namespace std;

//TAG pro distribuci vstupnich hodnot pomoci linearniho propojeni
#define TAG_INPUT 0
//TAG pro distribuci vstupnich hodnot pomoci sbernice
#define TAG_BUS 1
//TAG pro posilani serazenych hodnot danemu procesoru sbernici
#define TAG_RESULT_INTER 2
//TAG pro posilani vyslednych serazenych hodnot linearnim propojenim
#define TAG_RESULT_OUT 3

// Hodnoty kazdeho registru
struct Registers
{
   int X; //Vstupni hodnota -- BUS
   int Y; //Postupne vstupni data -- LIN
   int C; //Kolikrat X > Y
   int Z; //Vysledna hodnota
};

int main(int argc, char *argv[])
{
   //Pocet procesoru
   int numprocs;
   //Rank procesoru
   int myid;
   //Hodnota souseda pri posilani vysledne serazene posloupnosti
   int neighbour;
   //struct- obsahuje kod- source, tag, error
   MPI_Status stat;
   //Vstupni, neserazena posloupnost
   int * numbers;
   //Registry procesoru
   Registers values;
   //Serazena posloupnost
   int *sorted;
   //Nactene cislo ze souboru, prijate vysledne cislo, proc 0
   int number;

   //Hodnoty pro mereni casu
   double t1, t2;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);

   values.C = 1;

   //Nacteni souboru
   if(myid == 0)
   {
      //Jmeno souboru
      char input[]= "numbers";
      //Index vstupu
      int invar = 0;
      fstream fin;
      //Pole pro nactene hodnoty ze souboru
      numbers = new int[numprocs - 1];
      //Pole pro serazene hodnoty
      sorted = new int[numprocs - 1];

      fin.open(input, ios::in);
      if(!fin.is_open())
      {
         MPI_Abort(MPI_COMM_WORLD, 2);
      }

      while(fin.good())
      {
         number = fin.get();
         if(!fin.good())
            break;

         if(invar >= numprocs - 1)
         {
            MPI_Abort(MPI_COMM_WORLD, 2);
         }

         numbers[invar] = number;

         if(invar != 0)
            cout << " ";
         cout << number;
         invar++;
      }
      cout << endl;
      fin.close();
      if(invar != numprocs - 1)
      {
         MPI_Abort(MPI_COMM_WORLD, 2);
      }
   }

   //Mereni casu -- musime pockat na vytvoreni vsech procesu
   //MPI_Barrier(MPI_COMM_WORLD);
   //t1 = MPI_Wtime();

   if(myid == 0)
   {
      //Postupne posilame hodnoty ostatnim procesorum (linearnim
      //propojenim a sbernici)
      for(int i = 1; i < numprocs; i++)
      {
         MPI_Send(numbers + i - 1, 1, MPI_INT, i, TAG_BUS, MPI_COMM_WORLD);
         MPI_Send(numbers + i - 1, 1, MPI_INT, 1, TAG_INPUT, MPI_COMM_WORLD);
      }

      //Od posledniho procesoru prijmeme postupne serazene hodnoty
      for(int i = 0; i < numprocs - 1; i++)
      {
         MPI_Recv(&number, 1, MPI_INT, numprocs-1, TAG_RESULT_OUT, MPI_COMM_WORLD, &stat);
         sorted[i] = number;
      }
   }
   else
   {
      //Obrzime hodnotu od procesoru 0 a ulozime ji do registru X
      MPI_Recv(&(values.X), 1, MPI_INT, 0, TAG_BUS, MPI_COMM_WORLD, &stat);
      for(int i = 1; i < numprocs; i++)
      {
         //Obdrzime hodnotu od souseda myid - 1
         MPI_Recv(&(values.Y), 1, MPI_INT, myid - 1, TAG_INPUT, MPI_COMM_WORLD, &stat);
         //Podminka pro razeni stejnych hodnot
         if(i < myid && values.X >= values.Y)
         {
            values.C = values.C + 1;
         }
         else if(values.X > values.Y)
         {
            values.C = values.C + 1;
         }
         //Posleme linearnim spojenim hodnotu Y pravemu sousedovi myid+1
         if(myid < numprocs - 1)
         {
            MPI_Send(&(values.Y), 1, MPI_INT, myid + 1, TAG_INPUT, MPI_COMM_WORLD);
         }
      }
      //Posleme hodnotu X procesoru C (predstavuje spravnou pozici razene polozky)
      if(myid != values.C)
      {
         MPI_Send(&(values.X), 1, MPI_INT, values.C, TAG_RESULT_INTER, MPI_COMM_WORLD);
         MPI_Recv(&(values.Z), 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULT_INTER, MPI_COMM_WORLD, &stat);
      }
      else
      {
         //V pripade, ze bychom hodnotu poslali sami sobe, jen zkopirujeme promenne
         values.Z = values.X;
      }

      //Pomoci lineraniho spojeni posilame postupne serazenou hodnotu az do
      //procesoru 0 (posledni procesor posila zpet procesoru 0 -- master).
      for(int i = 1; i < numprocs; i++)
      {
         if(myid >= i)
         {
            neighbour = (myid + 1) % numprocs;
            MPI_Send(&(values.Z), 1, MPI_INT, neighbour, TAG_RESULT_OUT, MPI_COMM_WORLD);
         }
         if(myid > i)
         {
            MPI_Recv(&(values.Z), 1, MPI_INT, myid - 1, TAG_RESULT_OUT, MPI_COMM_WORLD, &stat);
         }

      }
   }

   //Ukoncime mereni casu
   //MPI_Barrier(MPI_COMM_WORLD);
   //t2 = MPI_Wtime();

   if(myid == 0)
   {
      //cout << t2 - t1 << endl;

      //Vypiseme serazene hodnoty
      for(int i = numprocs - 2; i >= 0 ; i--)
      {
         cout << sorted[i] << endl;
      }
      delete [] numbers;
      delete [] sorted;
   }


   MPI_Finalize();
   return 0;

}//main
