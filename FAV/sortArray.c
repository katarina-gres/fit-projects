extern int __VERIFIER_nondet_int();
const n = 3;
int main()
{
   int array[n];
   int ii = 0;
   int tmp = 0, i = 0, j = 0, maxInd  = 0;
   
   for(ii = 0; ii < n; ii++)
      array[ii] = __VERIFIER_nondet_int();
   
   for (i = 0; i < n - 1; i++) 
   {
      maxInd = i;
      for (j = i + 1; j < n; j++) 
      {
          if (array[j] > array[maxInd]) 
		maxInd = j;
      }
      tmp = array[i];
      array[i] = array[maxInd];
      array[maxInd] = tmp;
   } 
   for(ii = 0; ii < n - 1; ii++)
   {
      if(array[ii] < array[ii + 1])
         goto ERROR;
   }
   return 0;
   ERROR:
      return -1;
}
 
 
