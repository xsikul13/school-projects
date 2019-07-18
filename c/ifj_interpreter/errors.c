/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: errors.c
* Autor: Marcin Juraj
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#include "errors.h"
#include "scaner.h"

/**
 * vypis chyboveho hlaseni v zavislosti na typu
 */
void error(int e)
{
    switch (e)
    {
    case LEX_ERROR:
        fprintf(stderr,"Chyba v programe na riadku %d v ramci lexikalni analyzy (chybna struktura aktualneho lexemu).\n", current_line);
        break;

    case SYNTAX_ERROR:
        fprintf(stderr,"Chyba v programe na riadku %d v ramci syntakticke analyzy (chybna syntaxe struktury programu)\n", current_line);
        break;

    case SEM_ERROR_FUNCT_VAR:
        fprintf(stderr,"Chyba v programe na riadku %d "
            "v ramci semantickych kontrol (nedefinovanie alebo redefinicia funkcie alebo premennej)\n", current_line);
        break;

    case SEM_ERROR_ARITH_EXPR:
        fprintf(stderr,"Chyba v programe na riadku %d v ramci semantickych kontrol (aritmetickych vyrazoch)\n", current_line);
        break;

    case SEM_ERROR_OTHERS:
        fprintf(stderr,"Chyba v programe na riadku %d v ramci semantickych kontrol \n", current_line);
        break;

    case RUNTIME_ERROR_READ:
        fprintf(stderr,"Chyba vzniknuta za behu programe pri nacitani hodnoty zo vstupu \n");
        break;

    case RUNTIME_ERROR_VAR:
        fprintf(stderr,"Chyba vzniknuta za behu programe praca s neinicializovanou premennou \n");
        break;

    case RUNTIME_ERROR_DIVZERO:
        fprintf(stderr,"Chyba vzniknuta za behu programe delenie nulou \n");
        break;

    case RUNTIME_ERROR_OTHERS:
        fprintf(stderr,"Ina chyba vzniknuta za behu programe \n");
        break;

    case INTERPRET_ERROR:
        fprintf(stderr,"Interna chyba interpretu tj. neovplivnena vstupnim programem \n"
            "(napr. chyba alokace pameti, chyba pri otvirani vstupniho souboru atd.)\n");
        break;
    }        
}
