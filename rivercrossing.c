 /*
 * Projekt: IOS c.2 - River Crossing Problem
 * Autor:   Lukáš Drahník, xdrahn00@stud.fit.vutbr.cz
 * Datum:   22.4.2013
 * Soubor:  rcp.c
 * Popis programu: Program implementuje modifikovaný synchronizaèní problém River Crossing Problem..
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

/** Error codes */
enum tecodes
{
    EOK = 0,     /**< Without error */
    ECLWRONG,    /**< Wrong command line. */
    EUNKNOWN,    /**< Unknown error */
};

const char *HELPMSG =
"Program: IOS c.2 - River Crossing Problem\n"
"Autor: Lukas Drahnik, xdrahn00@stud.fit.vutbr.cz\n"
"Usage: riverscrossing -h\n"
"Arguments:\n";


/** Error messages */
const char *ECODEMSG[] =
{
  [ECLWRONG] = "Wrong parameters! Try -h.\n",
};

/** State codes */
enum tstates
{
    SHELP,
    STANDART,
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
    int state;         /**< State code => tstates. */
} TParams;

/**
 * Prototypes
 */
TParams getArgs(int argc, char *argv[]);
void printECode(int ecode);
bool isNumber(char *ch);
bool testArgs(TParams result);

/**
 * @param int argc
 * @param char *argv[]
 * @return TParams
 */
 TParams getArgs(int argc, char *argv[]) {
    TParams result = {
        .ecode = ECLWRONG,
        .state = STANDART,
    };

    if (argc == 2) {
        if (strcmp("-h", argv[1]) == 0) {
            result.state = SHELP;
            result.ecode = EOK;
        }
    } else if (argc == 5) {
        if (isNumber(argv[1]) && isNumber(argv[2]) && isNumber(argv[3]) && isNumber(argv[4])) {
            result.P = atoi(argv[1]);
            result.H = atoi(argv[2]);
            result.S = atoi(argv[3]);
            result.R = atoi(argv[4]);

            if (testArgs(result))
                result.ecode = EOK;
        }
    }
    return result;
}


/**
 * @param char *argv[]
 * @return bool
 */
bool testArgs(TParams result)
{
    if (result.P > 0 && (result.P % 2) == 0 &&
        result.H >= 0 && result.H < 5001 &&
        result.S >= 0 && result.S < 5001 &&
        result.R >= 0 && result.R < 5001) {
        return true;
    } else {
        return false;
    }
}

/**
 * @param int ecode
 * @return void
 */
void printECode(int ecode)
{
    if (ecode < EOK || ecode > EUNKNOWN) {
        ecode = EUNKNOWN;
    }
    fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * @param char *ch
 * @return bool
 */
bool isNumber(char *ch)
{
    int i = 0;
    while (ch[i] != '\0') {
        if (i == 0) {
            if(ch[0] == '.' || ch[0] ==  '-' || (ch[0] >= '0' && ch[0] <= '9')) {
                i++;
            }
            else {
                return false;
            }
        }
        else if ((ch[i] >= '0' && ch[i] <= '9') || ch[i] == '.') {
            i++;
        }
        else {
            return false;
        }
    }
    return true;
}

/**
 * Main program
 */
int main(int argc, char **argv)
{
    TParams params = getArgs(argc, argv);
    if(params.ecode != EOK) {
        printECode(params.ecode);
        return EXIT_FAILURE;
    }

    switch(params.state) {
        case SHELP:
            printf("%s\n", HELPMSG);
            break;
        case STANDART:
            printf("p = %d, h = %d, s = %d, r = %d\n", params.P, params.H, params.S, params.R);
            break;
    }
    return EXIT_SUCCESS;
}
