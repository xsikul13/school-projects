  // eratosthenes.c
  // Øešení IJC-DU1,  23.3.2014
  // Autor: Vojtìch Šikula, FIT
  // Pøeloženo: GNU gcc compiler
  // popis pøíkladu - poznámky, atd
#include <stdio.h>
#include "bit-array.h"
#include <math.h>
#include "eratosthenes.h"
#include "error.h"

void Eratosthenes(BitArray_t pole)
{
    unsigned long i = 2;
    unsigned long velikost = pole[0];
    unsigned long strop = (unsigned long ) sqrt(velikost)+1;


    for (i=2; i<strop; i++)
    {
        if ( GetBit(pole, i) == 0  )
        {
            unsigned long ii = i;
            while(ii < (velikost-i+1))
            {
                ii+=i;
		
                SetBit(pole,ii,1);
 
            }
        }
    }
}
