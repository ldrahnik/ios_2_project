 /**
  * Projekt: IOS c.2 - River Crossing Problem
  * Autor:   Lukáš Drahník, xdrahn00@stud.fit.vutbr.cz
  * Datum:   28.6.2020
  * Soubor:  rivercrossing.c
  * Popis programu: Program implementuje modifikovaný synchronizacní River Crossing Problem..
  */

#ifndef _rivercrossing_H_
#define _rivercrossing_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

/** Error codes */
enum tecodes
{
    EOK = 0,
    ECLWRONG,
    EUNKNOWN,
};

/** Error messages */
const char *ECODEMSG[] =
{
    [ECLWRONG] = "Wrong parameters! Try -h.\n",
};

/**
 * Struct of params
 */
typedef struct params
{
    int P;
    int H;
    int S;
    int R;
    int ecode;         /**< Error code => tecodes. */
} TParams;

/**
 * Prototypes
 */
TParams getArgs(int argc, char *argv[]);
void printECode(int ecode);
int isNumber(char *ch);
int testArgs(TParams result);
void waitingForAll();
void mainP(TParams params);
int startSemaphores();
int destroySemaphores();
int myRandom(int limit);
void hacker(TParams params, int i);
void serf(TParams params, int i);
void serfs(TParams params);
void hackers(TParams params);
void myPrintf(FILE *file, const char *message, ...);
void checkSetup();
void destroyAll();

#endif
