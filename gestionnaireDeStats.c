
#include "gestionnaireDeStats.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void logStats(char* msg)
{
    char test[100];
    bzero(test, sizeof(test));
    snprintf(test,500,"./scripts/log.sh ");
    strcat(test,msg);

    int r = system(test);
    if(r < 0)
        exit(1);
}