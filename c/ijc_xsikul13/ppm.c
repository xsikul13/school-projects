  // ppm.c
  // Øešení IJC-DU1,  23.3.2014
  // Autor: Vojtìch Šikula, FIT
  // Pøeloženo: GNU gcc compiler
  // Merlin se cuká
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
    struct ppm * ppm_read(const char * filename)
    {
        FILE *f;
        unsigned long xsize, ysize, rgb;
        int pom1, pom2;
        struct ppm *ppmstruct;
        if ( (f = fopen(filename, "rb"))  == NULL )
        {
            Warning("nepodarilo se otebrit soubor %s\n", filename);
            return NULL;
        }


        // nactení a kontrola hlavièky
        pom1 = fscanf(f, "P6 %lu %lu %lu", &xsize, &ysize, &rgb ); printf("xsize %lu   ysize %lu    rgb %lu", xsize, ysize, rgb);

        pom2 = fgetc(f);
        if( (pom1 != 3) || (rgb != 255) || (!isspace(pom2)) )
        {
            Warning("chyba formatu.\n");
            return NULL;
        }


        if ((ppmstruct = (struct ppm * ) malloc(2*sizeof(unsigned long)+ 3*xsize*ysize)) == NULL)
        {
            Warning("chyba alokace pameti\n");
            return NULL;
        }

        pom1 = fread(ppmstruct->data, 1, 3*xsize*ysize,  f);
        //printf("%lu pocet nactenech byte %lu", pom1, xsize*ysize*3);
        if (pom1 != 3*xsize*ysize)
        {
            Warning("nenacetl se cely soubor");
            free(ppmstruct);
            fclose(f);
            return NULL;
        }
        fclose(f);
        ppmstruct->xsize = xsize;
        ppmstruct->ysize = ysize;

        return ppmstruct;


    }

/// nezbyl èas
/*
    int ppm_write(struct ppm *p, const char * filename)
    {
        	;
    }
*/
