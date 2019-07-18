  // prvocisla.c
 // �e�en� IJC-DU1,  23.3.2014
  // Autor: Vojt�ch �ikula, FIT
  // P�elo�eno: GNU gcc compiler
  // popis p��kladu - pozn�mky, atd
#include <stdio.h>
#include <stdlib.h>
#include "eratosthenes.h"
#include "bit-array.h"
#include "error.h"
//#include "error.c"

#define POCETBITU 101000000lu
int main()
{
    
    //definice pole
    BitArray(pole, POCETBITU);
   




    Eratosthenes(pole);


    //tisk posledn�ch max 10 prvo��sel
    int pocet_prv = 10;
    unsigned long i;

    for(i=pole[0]; pocet_prv > 0; i--)
    {
        if (GetBit(pole, i) == 0)
        {
           pocet_prv--;
        }
    }
    while (pocet_prv != 10)
    {
        if(GetBit(pole,i) == 0)
        {
            printf("%lu\n", i);
            pocet_prv++;
        }
        i++;

    }



    return 0;
}


