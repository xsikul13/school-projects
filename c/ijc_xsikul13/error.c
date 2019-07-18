  // error.c
  // �e�en� IJC-DU1,  23.3.2014
  // Autor: Vojt�ch �ikula, FIT
  // P�elo�eno: GNU gcc compiler
  // popis p��kladu - pozn�mky, atd
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"


void Warning(const char *fmt, ...)
{
    va_list args;
 fprintf(stderr, "CHYBA: ");
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
}


void FatalError(const char *fmt, ...)
{
   va_list args;

    fprintf(stderr, "CHYBA: ");
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);

   exit(1);
}
