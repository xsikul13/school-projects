  // steg-decode.c
  // Øešení IJC-DU1,  23.3.2014
  // Autor: Vojtìch Šikula, FIT
  // Pøeloženo: GNU gcc compiler
  // popis pøíkladu - poznámky, atd
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "eratosthenes.h"
#include "bit-array.h"
#include "ppm.h"
#include "error.h"



// strýèek google pomohl s pøevrácením bitù >>> èasová tíseò
unsigned char otoc(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;
//   FILE *f;
//    f = fopen(argv[1],"rb" );

    struct ppm *ppmS = ppm_read(argv[1]);
    if (ppmS == NULL)
    {
        free(ppmS);
        FatalError("chyba se souborem ppm");
    }
    unsigned long vel = ppmS->ysize*ppmS->xsize*3;

    unsigned long *pole = calloc(sizeof(unsigned long), vel/sizeof(unsigned long)+1+(vel%sizeof(unsigned long)?0:1));
    pole[0]=vel;
    Eratosthenes(pole);


    unsigned char c = 0;
    int pom = 0;

    for (int i = 2; i < vel; i++)
    {


        if (GetBit(pole, i) == 0)
        {
           c = c << 1;
           c |= ppmS->data[i] & 1;
           pom++;

           if (pom == 8)
           {
               if (c == '\0')
               {
                   free(ppmS);
                   free(pole);
                   return 0;
               }/*
               if ( !isprint(c) )
               {
                   free(ppmS);
                   free(pole);
                   FatalError("netisknutelny znak");
               }*/

              putchar(otoc(c));
              c = 0;
              pom =0;
           }

           /*// printf("%d iii\n", i);
            c |= ((ppmS->data[i]&1)<<pom);
            pom--;

           if (pom == 0)
           {
               putchar(c);
               c=0;
               pom =7;
           }



            c = c<<1;
            //c |= ((ppmS->data[i])&(~(~0<<1)));
            c |= ((ppmS->data[i-1])%2);
            //c |= ((ppmS->data[i])&(~(~(char)0>>1)));
            pom++;//printf("%d %d\n " , pom, c);
        }

        if(pom == 8)
        {//printf("co se tady dìje?");
           // otoc(c);
           putchar(c);
            //putchar(otoc(c));
            if(c  == '\0')
                break;

            if (isprint(c))
            {
                putchar(c);
            }else
            {
                //free(ppmS);
               //FatalError("spatny format zpravy");
            }
            pom = 0;
            c=0;*/
        }



    }
    free(ppmS);
    free(pole);


    return 0;
}

