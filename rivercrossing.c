 /**
  * Projekt: IOS c.2 - River Crossing Problem
  * Autor:   Lukáš Drahník, xdrahn00@stud.fit.vutbr.cz
  * Datum:   22.4.2013
  * Soubor:  rivercrossing.c
  * Popis programu: Program implementuje modifikovaný synchronizacní River Crossing Problem..
  */

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

FILE *myFile;

sem_t *SEM_MOLO;
sem_t *SEM_MOLO_SERFS;
sem_t *SEM_MOLO_HACKERS;
sem_t *SEM_SHIP;
sem_t *SEM_SHIP_LANDING;
sem_t *SEM_WRITE;
sem_t *SEM_END;

int *COUNTER;
int *MOLO_SERFS;
int *MOLO_HACKERS;
int *SHIP_PERSONS;

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

void myPrintf(FILE *file, const char *message, ...) {
	va_list args;
	va_start(args, message);
	vfprintf(file, message, args);
	fflush(file);
	va_end(args);
    (*COUNTER)++;
}

/**
 * @param int ecode
 * @return void
 */
void printECode(int ecode) {
    if (ecode < EOK || ecode > EUNKNOWN) {
        ecode = EUNKNOWN;
    }
    fprintf(stderr, "%s", ECODEMSG[ecode]);
}

void hacker(TParams params, int i) {
    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        myPrintf(myFile, "%d: hacker: %d: started\n", *COUNTER, i);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }

    if (sem_wait(SEM_MOLO) == -1) {
        destroyAll();
    }

    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        (*MOLO_HACKERS)++;
        myPrintf(myFile, "%d: hacker: %d: waiting for boarding: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }
    checkSetup();
    if (sem_wait(SEM_MOLO_HACKERS) == -1) {
        destroyAll();
    }

    (*SHIP_PERSONS)++;
    if (*SHIP_PERSONS == 4) {
        *SHIP_PERSONS = 0;
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: hacker: %d: boarding: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
        int i;
        for (i =0; i < 4; i++) {
             if (sem_post(SEM_SHIP) == -1) {
                    destroyAll();
             }
        }
    } else {
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: hacker: %d: boarding: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
    }

    if (sem_wait(SEM_SHIP) == -1) {
        destroyAll();
    }
    (*SHIP_PERSONS)++;
    if (*SHIP_PERSONS == 4) {
        *SHIP_PERSONS = 0;
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: hacker: %d: captain \n", *COUNTER, i);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
        usleep(myRandom(params.R * 1000));
        int i;
        for (i =0; i < 4; i++) {
             if (sem_post(SEM_SHIP_LANDING) == -1) {
                    destroyAll();
             }
        }
    } else {
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: hacker: %d: member \n", *COUNTER, i);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
    }

    if (sem_wait(SEM_SHIP_LANDING) == -1) {
        destroyAll();
    }

    if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
    } else {
            myPrintf(myFile, "%d: hacker: %d: landing: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
    }

    (*SHIP_PERSONS)++;
    if (*SHIP_PERSONS == 4) {
        *SHIP_PERSONS = 0;
        sem_post(SEM_MOLO);
    }
    if (*COUNTER == params.P * 10) {
        int i;
        for (i = 0; i < params.P * 2; i++) {
             if (sem_post(SEM_END) == -1) {
                    destroyAll();
             }
        }
    }

    if (sem_wait(SEM_END) == -1) {
        destroyAll();
    }
    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        myPrintf(myFile, "%d: hacker: %d: finished\n", *COUNTER, i);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }
	exit(0);
}

void serf(TParams params, int i) {
    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        myPrintf(myFile, "%d: serf: %d: started\n", *COUNTER, i);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }

    if (sem_wait(SEM_MOLO) == -1) {
        destroyAll();
    }

    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        (*MOLO_SERFS)++;
        myPrintf(myFile, "%d: serf: %d: waiting for boarding: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }

    checkSetup();
    if (sem_wait(SEM_MOLO_SERFS) == -1) {
        destroyAll();
    }
    (*SHIP_PERSONS)++;
    if (*SHIP_PERSONS == 4) {
        *SHIP_PERSONS = 0;
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: serf: %d: boarding: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
        int i;
        for (i = 0; i < 4; i++) {
             if (sem_post(SEM_SHIP) == -1) {
                    destroyAll();
             }
        }
    } else {
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: serf: %d: boarding: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
    }

    if (sem_wait(SEM_SHIP) == -1) {
        destroyAll();
    }
    (*SHIP_PERSONS)++;
    if (*SHIP_PERSONS == 4) {
        *SHIP_PERSONS = 0;
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: serf: %d: captain \n", *COUNTER, i);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
        usleep(myRandom(params.R * 1000));
        int i;
        for (i =0; i < 4; i++) {
             if (sem_post(SEM_SHIP_LANDING) == -1) {
                    destroyAll();
             }
        }
    } else {
        if (sem_wait(SEM_WRITE) == -1) {
            destroyAll();
        } else {
            myPrintf(myFile, "%d: serf: %d: member \n", *COUNTER, i);
            if (sem_post(SEM_WRITE) == -1) {
                destroyAll();
            }
        }
    }

    if (sem_wait(SEM_SHIP_LANDING) == -1) {
        destroyAll();
    }
    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        myPrintf(myFile, "%d: serf: %d: landing: %d: %d \n", *COUNTER, i, *MOLO_HACKERS, *MOLO_SERFS);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }

    (*SHIP_PERSONS)++;
    if (*SHIP_PERSONS == 4) {
        *SHIP_PERSONS = 0;
        sem_post(SEM_MOLO);
    }

    if (*COUNTER == params.P * 10) {
        int i;
        for (i = 0; i < params.P * 2; i++) {
             if (sem_post(SEM_END) == -1) {
                    destroyAll();
             }
        }
    }
    if (sem_wait(SEM_END) == -1) {
        destroyAll();
    }
    if (sem_wait(SEM_WRITE) == -1) {
        destroyAll();
    } else {
        myPrintf(myFile, "%d: serf: %d: finished\n", *COUNTER, i);
        if (sem_post(SEM_WRITE) == -1) {
            destroyAll();
        }
    }
	exit(0);
}

/**
 * @param TParams params
 * Minor process which is starting serfs
 */
void serfs(TParams params) {
	int i, pid;
	for (i = 1; i <= params.P; i++) {
		usleep(myRandom(params.H * 1000));
		pid = fork();
		if (pid == 0) {
			serf(params, i);
		} else if (pid == -1) {
            destroyAll();
		}
	}
	waitingForAll();
	exit(0);
}

/**
 * @param TParams params
 * Minor process which is starting hackers
 */
void hackers(TParams params) {
	int i, pid;
	for (i = 1; i <= params.P; i++) {
		usleep(myRandom(params.H * 1000));
		pid = fork();
		if (pid == 0) {
			hacker(params, i);
		} else if (pid == -1) {
            destroyAll();
		}
	}
    waitingForAll();
	exit(0);
}

/**
 * @param int limit
 * @return int random
 */
int myRandom(int limit) {
    int random = 0;
	srand(time(NULL));
	if (limit == 0) {
        random = 0;
	} else {
		random += rand() % (limit + 1);
	}
	return random;
}

/**
 * Waiting for all child process
 */
void waitingForAll() {
	int pid;
    	while ((pid = waitpid (-1, NULL, 0))){
		if (errno == ECHILD) {
			break;
		}
	}
}

/**
 * Start all semaphores
 */
int startSemaphores() {
    SEM_MOLO = sem_open("xdr_MOLO", O_CREAT|O_EXCL, 0, 1);
	if (sem_unlink("xdr_MOLO") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
	SEM_MOLO_SERFS = sem_open("xdr_MOLO.SERFS", O_CREAT|O_EXCL, 0, 0);
	if (sem_unlink("xdr_MOLO.SERFS") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
	SEM_MOLO_HACKERS = sem_open("xdr_MOLO.HACKERS", O_CREAT|O_EXCL, 0, 0);
	if (sem_unlink("xdr_MOLO.HACKERS") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
    SEM_END = sem_open("xdr_END", O_CREAT|O_EXCL, 0, 0);
	if (sem_unlink("xdr_END") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
    SEM_WRITE = sem_open("xdr_WRITE", O_CREAT|O_EXCL, 0, 1);
	if (sem_unlink("xdr_WRITE") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
    SEM_SHIP = sem_open("xdr_SHIP", O_CREAT|O_EXCL, 0, 0);
	if (sem_unlink("xdr_SHIP") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
    SEM_SHIP_LANDING = sem_open("xdr_SHIP.LANDING", O_CREAT|O_EXCL, 0, 0);
	if (sem_unlink("xdr_SHIP.LANDING") == -1 || SEM_MOLO == SEM_FAILED) {
		return -1;
	}
	return 0;
}

/**
 * Destroy all semaphores
 */
int destroySemaphores() {
    if (sem_destroy(SEM_MOLO) == -1) {
		return -1;
	}
	if (sem_destroy(SEM_MOLO_SERFS) == -1) {
		return -1;
	}
	if (sem_destroy(SEM_MOLO_HACKERS) == -1) {
		return -1;
	}
	if (sem_destroy(SEM_END) == -1) {
		return -1;
	}
	if (sem_destroy(SEM_WRITE) == -1) {
		return -1;
	}
	if (sem_destroy(SEM_SHIP) == -1) {
		return -1;
	}
    if (sem_destroy(SEM_SHIP_LANDING) == -1) {
		return -1;
	}
	return 0;
}

/**
 * Destroy semaphores, close file and clear sharing memory
 */
void destroyAll() {
    if (destroySemaphores() == -1) {
		exit(2);
	}
    fclose(myFile);
    exit(2);
}

/**
 * Main process which is starting childs
 */
void mainP(TParams params) {
    myFile = fopen("rivercrossing.out", "w+");
    if (startSemaphores() == -1) {
        exit(2);
    }
	int shmid1;
   	key_t key;
	key = ftok(getenv("HOME"), 42);
	if ((shmid1 = shmget(key, sizeof(int), IPC_CREAT | 0660)) < 0) {
		exit(2);
	}
   	if ((COUNTER = shmat(shmid1, NULL, 0)) == (void*)-1) {
		exit(2);
	}
	int shmid2;
	key = ftok(getenv("HOME"), 52);
	if ((shmid2 = shmget(key, sizeof(int), IPC_CREAT | 0660)) < 0) {
		exit(2);
	}
   	if ((MOLO_SERFS = shmat(shmid2, NULL, 0)) == (void*)-1) {
		exit(2);
   	}
   	int shmid3;
	key = ftok(getenv("HOME"), 62);
	if ((shmid3 = shmget(key, sizeof(int), IPC_CREAT | 0660)) < 0) {
		exit(2);
	}
   	if ((MOLO_HACKERS = shmat(shmid3, NULL, 0)) == (void*)-1) {
		exit(2);
   	}
   	int shmid4;
    key = ftok(getenv("HOME"), 72);
	if ((shmid4 = shmget(key, sizeof(int), IPC_CREAT | 0660)) < 0) {
		exit(2);
	}
   	if ((SHIP_PERSONS = shmat(shmid4, NULL, 0)) == (void*)-1) {
		exit(2);
   	}

    *COUNTER = 1;
    *MOLO_SERFS = 0;
    *MOLO_HACKERS = 0;
    *SHIP_PERSONS = 0;

	int pid;
	pid = fork();
	if (pid == 0) {
		hackers(params);
	}
	pid = fork();
	if (pid == 0) {
		serfs(params);
	}
	waitingForAll();

    if (shmctl(shmid1, 0, IPC_RMID)) {
        exit(2);
    }
    if (shmctl(shmid2, 0, IPC_RMID)) {
        exit(2);
    }
    if (shmctl(shmid3, 0, IPC_RMID)) {
        exit(2);
    }
    if (shmctl(shmid4, 0, IPC_RMID)) {
        exit(2);
    }

    fclose(myFile);

    if (destroySemaphores() == -1) {
        exit(2);
    }
}

/**
 * @param int argc
 * @param char *argv[]
 * @return TParams result
 */
TParams getArgs(int argc, char *argv[]) {
	TParams result = {
		.ecode = ECLWRONG,
	};
    if (argc == 5) {
        if (isNumber(argv[1]) && isNumber(argv[2]) && isNumber(argv[3]) && isNumber(argv[4])) {
            result.P = atoi(argv[1]);
            result.H = atoi(argv[2]);
            result.S = atoi(argv[3]);
            result.R = atoi(argv[4]);
            if (testArgs(result)) {
                result.ecode = EOK;
            }
        }
    }
   	return result;
}

/**
 * Is checking persons setup to board on the boat
 */
void checkSetup() {
    int i;
    if (*MOLO_SERFS == 4) {
        (*MOLO_SERFS)-=4;
        for (i = 0; i < 4; i++) {
            if (sem_post(SEM_MOLO_SERFS) == -1) {
                destroyAll();
            }
        }
    } else if (*MOLO_HACKERS == 4) {
        (*MOLO_HACKERS)-=4;
        for (i = 0; i < 4; i++) {
            if (sem_post(SEM_MOLO_HACKERS) == -1) {
                destroyAll();
            }
        }
    } else if (*MOLO_SERFS >= 2 && *MOLO_HACKERS >= 2) {
        (*MOLO_SERFS)-=2;
        (*MOLO_HACKERS)-=2;
        for (i = 0; i < 2; i++) {
            if (sem_post(SEM_MOLO_SERFS) == -1) {
                destroyAll();
            }
        }
        for (i = 0; i < 2; i++) {
            if (sem_post(SEM_MOLO_HACKERS) == -1) {
                destroyAll();
            }
        }
    } else {
        sem_post(SEM_MOLO);
    }
 }

/**
 * @param TParams result
 * @return int
 */
int testArgs(TParams result) {
    if (result.P > 0 && (result.P % 2) == 0 &&
        result.H >= 0 && result.H < 5001 &&
        result.S >= 0 && result.S < 5001 &&
        result.R >= 0 && result.R < 5001) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @param char *ch
 * @return int
 */
int isNumber(char *ch) {
    char *errChar = 0;
    strtol(ch, &errChar, 10);
    if (errChar[0] == 0) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Main
 */
int main(int argc, char **argv) {
	TParams params = getArgs(argc, argv);
   	if(params.ecode != EOK) {
        	printECode(params.ecode);
       		return EXIT_FAILURE;
   	}
	mainP(params);
    return EXIT_SUCCESS;
}



