/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: errors.h
* Autor: Marcin Juraj
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>

enum errors
{
	OK = 0,               				 	// v poriadku
	LEX_ERROR = 1,         			   	// chyba lexikalnej analyzy
	SYNTAX_ERROR = 2,        				// chyba syntaktickej analyzy	
	SEM_ERROR_FUNCT_VAR = 3,        // chyba nedefinovanej funkcie alebo premennej alebo redefinicia f. alebo p.
	SEM_ERROR_ARITH_EXPR = 4,       // chyba v aritmetickych vyrazoch
	SEM_ERROR_OTHERS= 5,            // ostatne semanticke chyby
	RUNTIME_ERROR_READ= 6,          // runtimova chyba pri nacitani hodnoty zo vstupu
	RUNTIME_ERROR_VAR= 7,           // runtimova chyba praca s neinic. prem.
	RUNTIME_ERROR_DIVZERO= 8,       // runtimova chyba delenie 0
	RUNTIME_ERROR_OTHERS= 9,        // ostatne runtimove chyby
	INTERPRET_ERROR = 99,           // interna chyba interpretu prekladac
	
};

void error (int e);
/*char * StateToString(int i);
char * InstructionToString(int i);*/

#endif