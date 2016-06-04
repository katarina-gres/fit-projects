
/*
 * santa.c
 * Reseni 2. projektu do predmetu IOS, 29. 4. 2013
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define ERROR -1 /* Chybova konstanta. */
#define GROUP_SIZE 3 /* Velikost skupiny skritku. */

/* Chybove konstanty. */
typedef enum
{
   EOK,
   ESHARED,
   EFILE,
   ESEMAPHORE,
   EPARAMS,
   EFORMAT,
   ECREATEPROCESS,
   EUNKNOWN
} TERROR;

/* Sktruktura pro alokovani semaforu. */
typedef struct
{
   int semID;
   sem_t * sem;
} SEMITEM;

/* Kody jednotlivych semaforu. */
typedef enum
{
   OUTMUTEX,
   REQUESTMUTEX,
   SANTASLEEP,
   ELFMUTEX,
   ELFWAITINGQUEUE,
   ACTIVEELVESMUTEX,
   ELFHOLIDAY
} TSEMAPHORES;

/* Vstupni parametry programu. */
typedef struct
{
   TERROR error;
   unsigned int elfCount;
   unsigned int visitCount;
   unsigned int elfWork;
   unsigned int santaWork;
} TPARAMS;

/* Semafory */
sem_t * outMutex = NULL;
sem_t * requestMutex = NULL;
sem_t * santaSleep = NULL;
sem_t * elfMutex = NULL;
sem_t * elfWaitingQueue = NULL;
sem_t * activeElvesMutex = NULL;
sem_t * elfHoliday = NULL;

/* Pocitadlo vystupu. */
unsigned int * outCounter = NULL;
int outCounterID;
/* Pocet aktivnich elfu. */
int * activeElves = NULL;
int activeElvesID;
/* Pocet jeste neukoncenych sktritku. */
int * waitingForFinish = NULL;
int waitingForFinishID;

/* Vystupni souboru. */
FILE *outputFile = NULL;
int outputFileID;

/* Pole semaforu. */
SEMITEM semArray[7];
/* PID jednotlivych procesu, pocet jednotlivych procesu a PID rodice. */
pid_t *procArray;
unsigned int procCount = 0;
pid_t parent;

/* Chybove hlasky. */
const char * ERRORMSG[] = {
   "V poradku.",
   "Chyba pri vytvareni/mazani sdilene pameti.",
   "Chyba pri praci se soubory.",
   "Chyba pri praci se semafory.",
   "Chybne parametry programu.",
   "Chybny format parametru programu.",
   "Chyba pri vytvareni procesu.",
   "Neznama chyba."
};

/* Prototypy. */
int ShareMemory(int *varID, void **buffer, size_t size);  
void SantaProc(unsigned int visitCount, unsigned int elfCount,
   unsigned int wait);
void ElfProc(unsigned int order, unsigned int visitCount, 
	unsigned int elfCount, unsigned int wait);
int InitSemaphores(void);
int InitSharedVariables(void);
int FreeSemaphores(void);
int FreeSharedVariables(void);
void PrintError(int code);
TPARAMS GetParams(int argc, char *argv[]);
void AssignSemaphores(void);
void FreeResources(int main);
void SignalHandler(int sig);
int DeatachVariables(void);
int AtachVariables(void);
int DestroySemaphores(void);
int CreateSemaphores(void);

int main(int argc, char *argv[])
{
   TPARAMS param = GetParams(argc, argv);
   if(param.error != EOK)
   {
      PrintError(param.error);
      return 1;
   }
   
   signal(SIGTERM, SignalHandler);
   signal(SIGINT, SignalHandler);
   signal(SIGUSR1, SignalHandler);
   
   int state = EOK;
   if((state = CreateSemaphores()) != EOK)
   {
      PrintError(state);
      FreeResources(0);
      return 2;
   }
   if((state = InitSharedVariables()) != EOK)
   {
      PrintError(state);
      FreeResources(0);
      return 2;
   }
   if((state = AtachVariables()) != EOK)
   {
      PrintError(state);
      FreeResources(0);
      return 2;
   }
   if((state = InitSemaphores()) != EOK)
   {
      PrintError(state);
      FreeResources(0);
      return 2;
   }
   AssignSemaphores();
   
   if((outputFile = fopen("santa.out", "w")) == NULL)
   {
      PrintError(EFILE);
      FreeResources(0);
      return 2;
   }
   setbuf(outputFile, NULL);
   
   parent = getpid();
   *waitingForFinish = 0;
   *activeElves = param.elfCount;
   *outCounter = 0;
   
   srand(time(0));
   
   procCount = param.elfCount + 1;
   if((procArray = malloc((param.elfCount + 1) * sizeof(sem_t))) == NULL)
   {
      PrintError(ESHARED);
      FreeResources(0);
      return EXIT_FAILURE;
   }
   for(unsigned int i = 0; i < param.elfCount + 1; i++)
      procArray[i] = 0;
   pid_t desc;
   for(unsigned int i = 0; i < param.elfCount + 1; i++)
   {
      desc = fork();
      if(desc == 0)
      {
         if((state = AtachVariables()) != EOK)
         {
            PrintError(state);
            kill(parent, SIGUSR1);
            //exit(2);
         }
         AssignSemaphores();
         
         if(i == 0)
         {
            SantaProc(param.visitCount, param.elfCount, 
               param.santaWork);
         }
         else
         {
            ElfProc(i, param.visitCount, param.elfCount, 
               param.elfWork);
         }
         FreeResources(1);
         exit(0);
      }
      else if(desc > 0)
      {
         procArray[i] = desc;
      }
      else if(desc == ERROR)
      {
         for(unsigned int j = 0; j < i; j++)
         {
            kill(procArray[j], SIGTERM);
         }
         PrintError(ECREATEPROCESS);
         FreeResources(0);
         exit(2);
      }
   }
   
   for(unsigned int i = 0; i < param.elfCount + 1; i++)
   {
      waitpid(procArray[i], NULL, 0);
   }
   
   FreeResources(0);

   return 0;
}

/* 
 * Funkce, ktera simuluje chovani santy.
 * @param visitCount Pocet navstev jednoho skritka.
 * @param elfCount Pocet skritku.
 * @param wait Doba cekani santy.
 */
void SantaProc(unsigned int visitCount, unsigned int elfCount,
   unsigned int wait)
{
   int elves = elfCount * visitCount; /* Pocet zpracovanych skritku. */
   
   if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      (*outCounter)++;
      fprintf(outputFile, "%u: santa: started\n", *outCounter);
      fflush(outputFile);
   if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   
   while(elves > 0)
   {
      /* Santa pocka na pozadavek. */
      if(sem_wait(requestMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); } 
      
      if(sem_wait(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
            (*outCounter)++;
            fprintf(outputFile, "%u: santa: checked state: %d: %d\n", *outCounter, 
               *activeElves, *waitingForFinish); 
            fflush(outputFile);
         if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         if(*activeElves > 3 && *waitingForFinish < 3)
         {
            sem_post(activeElvesMutex); /* Santa ceka na dalsi pozadavek. */
            continue;
         }
      if(sem_post(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      /* Nyni santa muze splnit pozadavek */
      if(sem_wait(santaSleep)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); } 
      
      if(sem_wait(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
            (*outCounter)++;
            fprintf(outputFile, "%u: santa: can help\n", *outCounter);
            fflush(outputFile);
         if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         if(wait > 0)
            usleep((rand() % wait) * 1000);
         
         elves -= *waitingForFinish;
         for(int i = 0; i < *waitingForFinish; i++)
         {
            if(sem_post(elfWaitingQueue)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         }
      if(sem_post(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   }
   
   if(sem_wait(elfHoliday)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      (*outCounter)++;
      fprintf(outputFile, "%u: santa: checked state: %d: %d\n", *outCounter, 
         *activeElves, *waitingForFinish);
      fflush(outputFile);
      (*outCounter)++;
      fprintf(outputFile, "%u: santa: finished\n", *outCounter);
      fflush(outputFile);
   if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
}

/*
 * Funkce, ktera simuluje cinnost elfa.
 * @param order Poradi elfa.
 * @param visitCount Pocet navstev.
 * @param elfCount Celkovy pocet elfu.
 * @param wait Doba cekani.
 */
void ElfProc(unsigned int order, unsigned int visitCount, 
	unsigned int elfCount, unsigned int wait)
{
   if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      (*outCounter)++;
      fprintf(outputFile, "%u: elf: %u: started\n", *outCounter, order);
      fflush(outputFile);
   if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   
   for(unsigned int i = 0; i < visitCount; i++)
   {
      /*Uspani skritka na nahodny cas.*/
      if(wait > 0)
         usleep((rand() % wait) * 1000);
      if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         (*outCounter)++;
         fprintf(outputFile, "%u: elf: %u: needed help\n", *outCounter, order);
         fflush(outputFile);
      if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      /* Davkovani skritku. */
      if(sem_wait(elfMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); } 
      if(sem_wait(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
            (*outCounter)++;
            fprintf(outputFile, "%u: elf: %u: asked for help\n", *outCounter, order);
            fflush(outputFile);
         if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         
         (*waitingForFinish)++;
         
         if(*activeElves > 3 && *waitingForFinish == 3)
         {
            /* Povolime vstup k santovi. */
            if(sem_post(requestMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); } 
            if(sem_post(santaSleep)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         }
         else if(*activeElves <= 3)
         {
            if(sem_post(requestMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
            if(sem_post(santaSleep)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         }
         else
         {
            /* Pustime dalsiho skritka. */
            if(sem_post(elfMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); } 
         }
      if(sem_post(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      /* Pockej na ukonceni obsluhy. */
      if(sem_wait(elfWaitingQueue)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); } 
      if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         (*outCounter)++;
         fprintf(outputFile, "%u: elf: %u: got help\n", *outCounter, order);
         fflush(outputFile);
      if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      
      if(sem_wait(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         if(i == visitCount - 1)
         {
            if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
               (*outCounter)++;
               fprintf(outputFile, "%u: elf: %u: got a vacation\n", *outCounter, order);
               fflush(outputFile);
            if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
            (*activeElves)--;
         }
         (*waitingForFinish)--;
         if(*waitingForFinish == 0)
         {
            if(sem_post(elfMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         }
      if(sem_post(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   }

   if(sem_wait(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      if(*activeElves == 0)
      {
         for(unsigned int i = 0; i < elfCount + 1; i++)
         {
            if(sem_post(elfHoliday)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
         }
      }
   if(sem_post(activeElvesMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   if(sem_wait(elfHoliday)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
   if(sem_wait(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
      (*outCounter)++;
      fprintf(outputFile, "%u: elf: %u: finished\n", *outCounter, order);
      fflush(outputFile);
   if(sem_post(outMutex)==ERROR) { PrintError(ESEMAPHORE); kill(parent, SIGUSR1); }
}

/*
 * Funkce, ktera vytvori sdilenou promennou.
 * @param varID ID promenne.
 * @param buffer Uloziste promenne.
 * @param size Velikost promenne.
 * @return EOK, v pripade chyby ESHARED.
 */
int ShareMemory(int *varID, void **buffer, size_t size)
{
   if((*varID = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0666)) == ERROR)
   {
      return ESHARED;
   }
   if((*buffer = shmat(*varID, NULL, 0)) == NULL)
   {
      return ESHARED;
   }
   return EOK;
}

/*
 * Funkce, ktera vytvori sdilene semafory.
 * @return EOK, v pripade chyby ESEMAPHORE. 
 */
int InitSemaphores(void)
{   
   int initArray[7] = {1, 0, 0, 1, 0, 1, 0};
   for(int i = 0; i < 7; i++)
   {
      if(sem_init(semArray[i].sem, 1, initArray[i]) != EOK)
      {
         return ESEMAPHORE;
      }
   }
   
   return EOK;
}

/*
 * Funkce, ktera vytvori sdilenou pamet pro semafory.
 * @return EOK, v pripade chyby ESHARED.
 */
int CreateSemaphores(void)
{
   for(int i = 0; i < 7; i++)
   {
      if((semArray[i].semID = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 
         IPC_EXCL | 0666)) == ERROR)
      {
         return ESHARED;
      }
   }
   
   return EOK;
}

/*
 * Funkce, ktera inicializuje sdilene promenne.
 * @return EOK, v pripade chyby ESHARED.
 */
int InitSharedVariables(void)
{
   if((outCounterID = shmget(IPC_PRIVATE, sizeof(unsigned int), IPC_CREAT | 
      IPC_EXCL | 0666)) == ERROR)
   {
      return ESHARED;
   }
   if((activeElvesID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 
      IPC_EXCL | 0666)) == ERROR)
   {
      return ESHARED;
   }
   if((waitingForFinishID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 
      IPC_EXCL | 0666)) == ERROR)
   {
      return ESHARED;
   }

   return EOK;
}

/*
 * Funkce, ktera uvolni semafory.
 * @return EOK, v pripade chyby ESEMAPHORE.
 */
int FreeSemaphores(void)
{
   for(int i = 0; i < 7; i++)
   {
      if(semArray[i].sem == NULL)
         continue;
      if(shmctl(semArray[i].semID, IPC_RMID, NULL) == ERROR)
         return ESEMAPHORE;
   }
   
   return EOK;
}

/*
 * Funkce, ktera uvolni sdilene promenne.
 * @return EOK, v pripade chyby ESHARED.
 */
int FreeSharedVariables(void)
{
   if(outCounter != NULL && shmctl(outCounterID, IPC_RMID, NULL) == ERROR)
   {
      return ESHARED;
   }
   if(activeElves != NULL && shmctl(activeElvesID, IPC_RMID, NULL) == ERROR)
   {
      return ESHARED;
   }
   if(waitingForFinish != NULL && shmctl(waitingForFinishID, IPC_RMID, NULL) == ERROR)
   {
      return ESHARED;
   }
   
   return EOK;
}

/*
 * Funkce, ktera vytiskne chybovou hlasku.
 * @param code Cislo chyby.
 */
void PrintError(int code)
{
   if(code < EOK || code > EUNKNOWN)
      code = EUNKNOWN;
   printf("%s\n", ERRORMSG[code]);
}

/*
 * Funkce, ktera zpracuje parametry prikazove radky.
 * @param argc Pocet Parametru.
 * @param argv Jednotlive parametry.
 * @return Nactene parametry.
 */
TPARAMS GetParams(int argc, char *argv[])
{
   TPARAMS ret = { EOK, 0, 0, 0, 0 };
   
   if(argc != 5)
   {
      ret.error = EPARAMS;
      return ret;
   }
   
   char * err = NULL;
   ret.visitCount = strtoul(argv[1], &err, 10);
   if(*err != '\0')
   {
      ret.error = EFORMAT;
      return ret;
   }
   ret.elfCount = strtoul(argv[2], &err, 10);
   if(*err != '\0' || !isdigit(argv[2][0]))
   {
      ret.error = EFORMAT;
      return ret;
   }
   ret.elfWork = strtoul(argv[3], &err, 10);
   if(*err != '\0' || !isdigit(argv[3][0]))
   {
      ret.error = EFORMAT;
      return ret;
   }
   ret.santaWork = strtoul(argv[4], &err, 10);
   if(*err != '\0' || !isdigit(argv[4][0]))
   {
      ret.error = EFORMAT;
      return ret;
   }
   
   if(ret.visitCount == 0 || ret.elfCount == 0)
   {
      ret.error = EFORMAT;
      return ret;
   }

   return ret;
}

/*
 * Funkce, ktera priradi jednotlivym semaforum 
 * nactene pole semaforu.
 */
void AssignSemaphores(void)
{
   outMutex = semArray[0].sem;
   requestMutex = semArray[1].sem;
   santaSleep = semArray[2].sem;
   elfMutex = semArray[3].sem;
   elfWaitingQueue = semArray[4].sem;
   activeElvesMutex = semArray[5].sem;
   elfHoliday = semArray[6].sem;
}

/*
 * Funkce, ktera uvolni zdroje pouzivane programem.
 */
void FreeResources(int main)
{
   if(outputFile != NULL)
      fclose(outputFile);
   if(procArray != NULL)
      free(procArray);
   
   int state = EOK;
   if((state = DeatachVariables()) != EOK)
   {
      PrintError(state);
   }
   if(main == 0)
   {
      if((state = DestroySemaphores()) != EOK)
      {
         PrintError(state);
      }
      if((state = FreeSemaphores()) != EOK)
      {
         PrintError(state);
      }
      if((state = FreeSharedVariables()) != EOK)
      {
         PrintError(state);
      }
   }
}

/*
 * Funkce, ktera pripoji promenne ke sdilene pameti.
 * @return EOK, v pripade chyby ESHARED
 */
int AtachVariables(void)
{
   for(int i = 0; i < 7; i++)
   {
      if((semArray[i].sem = (sem_t *)shmat(semArray[i].semID, 
         NULL, 0)) == NULL)
      {
         return ESHARED;
      }
   }
   
   if((outCounter = shmat(outCounterID, NULL, 0)) == NULL)
   {
      return ESHARED;
   }
   if((activeElves = shmat(activeElvesID, NULL, 0)) == NULL)
   {
      return ESHARED;
   }
   if((waitingForFinish = shmat(waitingForFinishID, NULL, 0)) == NULL)
   {
      return ESHARED;
   }
   
   return EOK;
}

/*
 * Funkce, ktera znici semafory.
 * @return EOK, v pripade chyby ESEMAPHORE
 */
int DestroySemaphores(void)
{
   for(int i = 0; i < 7; i++)
   {
      if(semArray[i].sem == NULL)
         continue;
      if(sem_destroy(semArray[i].sem) == ERROR)
         return ESEMAPHORE;
   }
   return EOK;
}

/*
 * Funkce, ktera odpoji promenne od sdilene pameti.
 * @return EOK, v pripade chyby ESHARED
 */
int DeatachVariables(void)
{
   for(int i = 0; i < 7; i++)
   {
      if(semArray[i].sem == NULL)
         continue;
      if(shmdt(semArray[i].sem) == ERROR)
         return ESHARED;
   }
   if(outCounter != NULL && shmdt(outCounter) == ERROR)
   {
      return ESHARED;
   }
   if(activeElves != NULL && shmdt(activeElves) == ERROR)
   {
      return ESHARED;
   }
   if(waitingForFinish != NULL && shmdt(waitingForFinish) == ERROR)
   {
      return ESHARED;
   }
   
   return EOK;
}

/*
 * Funkce, ktera zachytava signaly v programu.
 * @param sig Signal
 */
void SignalHandler(int sig)
{  
   if(sig == SIGUSR1)
   {
      for(unsigned int i = 0; i < procCount; i++)
      {
         if(procArray[i] != 0)
            kill(procArray[i], SIGTERM);
      }
      FreeResources(0);
   }
   else if(sig == SIGTERM)
   {
      FreeResources(1);
   }
   else if(sig == SIGINT)
   {
      if(parent == getpid())
      {
         FreeResources(0);
      }
      else
      {
         FreeResources(1);
      }
   }
   
   exit(2);
}
