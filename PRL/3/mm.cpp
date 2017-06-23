/*
 * Projekt do predmetu PRL: Mesh multiplication
 * Autor: Vojtech Havlena, xhavle03@stud.fit.vutbr.cz
 *
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

//TAG pro uvodni poslani rozmeru (pocet radku/sloupcu vysledne matice)
#define TAG_DIMENSION_ROWS 1
#define TAG_DIMENSION_COLS 2
//Pocet prvku, ktere je treba v kazdem procesoru zpracovat
#define TAG_WORK_ITEMS 3
//TAG zpravy obsahujici radky/sloupce vstupni matice pro krajni procesory
#define TAG_INPUT_COL 5
#define TAG_INPUT_ROW 6
//TAG zpravy pro poslani hodnoty pravemu sousedovi (pri samotnem algoritmu)
#define TAG_LEFT 7
//TAG zpravy pro poslani hodnoty spodnimu sousedovi (pri samotnem algoritmu)
#define TAG_TOP 8
//Zprava nesouci vysledek (hodnotu C) posilana procesoru P_0
#define TAG_RES 9

//Chybove kody
enum ERROR
{
   OK,
   DIMENSION,
   ONLY_NUMBERS,
   SHAPE,
   ROWS,
   COLS
};

//Struktura reprezentujici matici
struct Matrix
{
   vector< vector<int> > data;
   int rows;
   int cols;
};

//Chybove hlasky
const char * ERR_MSG[] = {
   "OK",
   "Rozmery musi byt kladna cela cisla.",
   "Matice musi obsahovat pouze cisla.",
   "Matice musi mit obdelnikovy tvar.",
   "Nesouhlasi pocet radku.",
   "Nesouhlasi pocet sloupcu."
};

Matrix getMatrix(ifstream & file, bool firstRow, ERROR * err);
Matrix initMatrix(int rows, int cols);
void printMatrix(Matrix mtx);

//Funkce pro vytisknuti matice na stdout v zadanem formatu.
void printMatrix(Matrix mtx)
{
   cout << mtx.rows << ":" << mtx.cols << "\n";
   for(int i = 0; i < mtx.rows; i++)
   {
      for(int j = 0; j < mtx.cols; j++)
      {
         if(j != 0)
            cout << " ";
         cout << mtx.data[i][j];
      }
      cout << "\n";
   }
}

//Funkce pro inicializaci matice na dany pocet radku a sloupcu
Matrix initMatrix(int rows, int cols)
{
   Matrix ret;
   ret.rows = rows;
   ret.cols = cols;
   ret.data = vector<vector<int>>(rows);
   for(int i = 0; i < rows; i++)
      ret.data[i] = vector<int>(cols);

   return ret;
}

//Funkce pro nacteni matice ze souboru (je rovnez provadena kontrola
//vstupniho formatu matice)
Matrix getMatrix(ifstream & file, bool firstRow, ERROR * err)
{
   int number;
   int constraint;
   int cols = -1;
   Matrix ret;
   string line;

   *err = OK;

   getline(file, line);
   stringstream initStream(line);
   if(initStream.good())
   {
      initStream >> constraint;
      if(initStream.fail() && !initStream.eof())
      {
         *err = DIMENSION;
         return ret;
      }
      if(constraint < 0)
      {
         *err = DIMENSION;
         return ret;
      }
   }
   else
   {
      *err = DIMENSION;
      return ret;
   }

   while(getline(file, line))
   {
      stringstream lineStream(line);
      vector<int> rowVec;

      while(lineStream >> number)
      {
         rowVec.push_back(number);
      }
      if(lineStream.fail() && !lineStream.eof())
      {
         *err = ONLY_NUMBERS;
         return ret;
      }

      if(cols == -1)
      {
         cols = rowVec.size();
         ret.cols = cols;
      }
      if(cols != rowVec.size())
      {
         *err = SHAPE;
         return ret;
      }
      ret.data.push_back(rowVec);
   }

   ret.rows = ret.data.size();
   if(firstRow && ret.data.size() != constraint)
   {
      *err = ROWS;
      return ret;
   }
   else if(!firstRow && cols != constraint)
   {
      *err = COLS;
      return ret;
   }

   return ret;
}

int main(int argc, char *argv[])
{
   //Pocet procesoru
   int numprocs;
   //Rank procesoru
   int myid;
   //struct- obsahuje kod- source, tag, error
   MPI_Status stat;

   //Hodnoty pro mereni casu
   double t1, t2;

   //Rozmery vstupnich matic
   int workItems, dimRows, dimCols;
   //Vysledek
   int C = 0;
   //Vstupni hodnoty kazdeho procesoru behem algoritmu
   int A, B;
   //Pole pro zaslani vysledku (ID procesoru, hodnota).
   int res[2];
   //Vysledna vynasobena matice
   Matrix mulMtx;

   //Radek matice, ktery je ulozen v krajnim procesoru (svislem)
   int * rowBuffer = NULL;
   //Sloupec matice, ktery je ulozen v krajnim procesoru (vodorovnem)
   int * colBuffer = NULL;
   //Aktualne zpracovavany prvek v rowBufferu
   int indexRow = 0;
   //Aktualne zpracovavany prvek v colBufferu
   int indexCol = 0;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);


   //Nacteni souboru a rozesilani nezbytnych hodnot ostatnim procesorum
   if(myid == 0)
   {
      ifstream mat1File;
      ifstream mat2File;

      mat1File.open("mat1", ifstream::in);
      mat2File.open("mat2", ifstream::in);
      if(!mat1File.is_open() || !mat2File.is_open())
      {
         cerr << "Chyba pri otevirani vstupniho souboru." << endl;
         MPI_Abort(MPI_COMM_WORLD, 2);
      }

      //Parsovani souboru mat1
      ERROR err;
      Matrix numbersMtx1;
      Matrix numbersMtx2;
      numbersMtx1 = getMatrix(mat1File, true, &err);
      if(err != OK)
      {
         mat1File.close();
         mat2File.close();
         cerr << ERR_MSG[err] << endl;
         MPI_Abort(MPI_COMM_WORLD, 2);
      }

      //Parsovani souboru mat2
      numbersMtx2 = getMatrix(mat2File, false, &err);
      if(err != OK)
      {
         mat1File.close();
         mat2File.close();
         cerr << ERR_MSG[err] << endl;
         MPI_Abort(MPI_COMM_WORLD, 2);
      }
      mat1File.close();
      mat2File.close();

      //Kontrola, jestli rozmery matic umoznuji provest nasobeni
      if(numbersMtx1.cols != numbersMtx2.rows)
      {
         cerr << "Nekompatibilni matice pro nasobeni." << endl;
         MPI_Abort(MPI_COMM_WORLD, 2);
      }

      workItems = numbersMtx1.cols;
      dimRows = numbersMtx1.rows;
      dimCols = numbersMtx2.cols;

      mulMtx = initMatrix(dimRows, dimCols);

      //Radek matice numbersMtx1
      int *row = new(nothrow) int[workItems];
      //Sloupec matice numbersMtx1
      int *col = new(nothrow) int[workItems];

      //Procesor 0 si alokuje prostor pro ulozeni hodnot
      rowBuffer = new(nothrow) int[workItems];
      colBuffer = new(nothrow) int[workItems];
      if(row == NULL || col == NULL || rowBuffer == NULL || colBuffer == NULL)
      {
         cerr << "Chyba pri alokaci pameti." << endl;
         MPI_Abort(MPI_COMM_WORLD, 2);
      }
      //Nastaveni indexu prave zpracovavanych hodnot
      indexRow = workItems - 1;
      indexCol = workItems - 1;

      //Poslani rozmeru vysledne matice a pocet ocekavanych hodnot vsem procesorum
      for(int i = 1; i < numprocs; i++)
      {
         MPI_Send(&dimRows, 1, MPI_INT, i, TAG_DIMENSION_ROWS, MPI_COMM_WORLD);
         MPI_Send(&dimCols, 1, MPI_INT, i, TAG_DIMENSION_COLS, MPI_COMM_WORLD);
         MPI_Send(&workItems, 1, MPI_INT, i, TAG_WORK_ITEMS, MPI_COMM_WORLD);
      }
      //Poslani sloupcu matice vodorovnym krajnim procesorum
      for(int i = 0; i < dimCols; i++)
      {
         for(int j = 0; j < workItems; j++)
         {
            col[j] = numbersMtx2.data[j][i];
            if(i == 0)
               colBuffer[j] = numbersMtx2.data[j][i];
         }
         if(i != 0)
            MPI_Send(col, numbersMtx2.rows, MPI_INT, i, TAG_INPUT_COL, MPI_COMM_WORLD);
      }
      //Poslani radku matice svislym krajnim procesorum
      for(int i = 0; i <= (dimRows-1)*dimCols; i += dimCols)
      {
         for(int j = 0; j < workItems; j++)
         {
            row[j] = numbersMtx1.data[i/dimCols][j];
            if(i == 0)
               rowBuffer[j] = numbersMtx1.data[i/dimCols][j];
         }
         if(i != 0)
            MPI_Send(row, numbersMtx1.cols, MPI_INT, i, TAG_INPUT_ROW, MPI_COMM_WORLD);
      }

      delete [] row;
      delete [] col;
   }

   //Ostatni procesory ziskaji rozmery matice, krajni procesory i odpovidajici
   //radky/soupce vstupnich matic
   if(myid != 0)
   {
      //Nacteni poctu radku vysledne matice
      MPI_Recv(&dimRows, 1, MPI_INT, 0, TAG_DIMENSION_ROWS, MPI_COMM_WORLD, &stat);
      //Nacteni poctu sloupcu vysledne matice
      MPI_Recv(&dimCols, 1, MPI_INT, 0, TAG_DIMENSION_COLS, MPI_COMM_WORLD, &stat);
      //Nacteni poctu radku, ktere projdou procesorem
      MPI_Recv(&workItems, 1, MPI_INT, 0, TAG_WORK_ITEMS, MPI_COMM_WORLD, &stat);

      //Krajni procesory si nactou i hodnoty
      //Vodorovne krajni procesory
      if(myid < dimCols)
      {
         colBuffer = new(nothrow) int[workItems];
         if(colBuffer == NULL)
         {
            cerr << "Chyba pri alokaci pameti." << endl;
            MPI_Abort(MPI_COMM_WORLD, 2);
         }
         //Prijmeme cely sloupec
         MPI_Recv(colBuffer, workItems, MPI_INT, 0, TAG_INPUT_COL, MPI_COMM_WORLD, &stat);
         indexCol = workItems - 1;
      }
      //Svisle krajni procesory
      if(myid % dimCols == 0)
      {
         rowBuffer = new(nothrow) int[workItems];
         if(rowBuffer == NULL)
         {
            cerr << "Chyba pri alokaci pameti." << endl;
            MPI_Abort(MPI_COMM_WORLD, 2);
         }
         //Prijmeme cely radek
         MPI_Recv(rowBuffer, workItems, MPI_INT, 0, TAG_INPUT_ROW, MPI_COMM_WORLD, &stat);
         indexRow = workItems - 1;
      }
   }

   //MPI_Barrier(MPI_COMM_WORLD);
   //t1 = MPI_Wtime();

   //Samotny vypocet nasobeni matic
   for(int i = 0; i < workItems; i++)
   {
      //Ziskame nove hodnoty A,B pro aktualizaci hodnoty C (zalezi, jesli je
      //procesor krajni nebo vnitrni)
      if(myid < dimCols)
      {
         if(myid == 0)
         {
            A = rowBuffer[indexRow];
            indexRow--;
         }
         else
         {
            MPI_Recv(&A, 1, MPI_INT, myid - 1, TAG_LEFT, MPI_COMM_WORLD, &stat);
         }
         B = colBuffer[indexCol];
         indexCol--;
         C = C + A*B;
      }
      else if(myid % dimCols == 0)
      {
         A = rowBuffer[indexRow];
         indexRow--;
         MPI_Recv(&B, 1, MPI_INT, myid - dimCols, TAG_TOP, MPI_COMM_WORLD, &stat);
         C = C + A*B;
      }
      else
      {
         MPI_Recv(&A, 1, MPI_INT, myid - 1, TAG_LEFT, MPI_COMM_WORLD, &stat);
         MPI_Recv(&B, 1, MPI_INT, myid - dimCols, TAG_TOP, MPI_COMM_WORLD, &stat);
         C = C + A*B;
      }
      //Posleme sousedovi dolu
      if(myid + dimCols < numprocs)
      {
         MPI_Send(&B, 1, MPI_INT, myid + dimCols, TAG_TOP, MPI_COMM_WORLD);
      }
      //Posleme sousedovi vlevo
      if((myid + 1) % dimCols != 0)
      {
         MPI_Send(&A, 1, MPI_INT, myid + 1, TAG_LEFT, MPI_COMM_WORLD);
      }
   }

   //MPI_Barrier(MPI_COMM_WORLD);
   //t2 = MPI_Wtime();

   //Ziskame spocitane hodnoty C a procesor 0 je ulozi do matice
   if(myid == 0)
   {
      mulMtx.data[0][0] = C;
      for(int i = 1; i < numprocs; i++)
      {
         MPI_Recv(res, 2, MPI_INT, i, TAG_RES, MPI_COMM_WORLD, &stat);
         mulMtx.data[res[0]/dimCols][res[0]%dimCols] = res[1];
      }
      //cout << t2 - t1 << endl;
      //Vytiskne vysledek
      printMatrix(mulMtx);
   }
   else
   {
      //Posleme vysledek procesoru 0
      res[0] = myid;
      res[1] = C;
      MPI_Send(res, 2, MPI_INT, 0, TAG_RES, MPI_COMM_WORLD);
   }

   if(myid < dimCols)
   {
      delete [] colBuffer;
   }
   if(myid % dimCols == 0)
   {
      delete [] rowBuffer;
   }

   MPI_Finalize();
   return 0;

}//main
