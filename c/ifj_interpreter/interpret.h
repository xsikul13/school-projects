/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: interpret.h
* Autor: Šikula Vojtěch
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#ifndef INTERPRET_H
#define INTERPRET_H


#include "ilist.h"
#include <stdio.h>
#include "str.h"
#include "errors.h"
#include "ial.h"
#include "sa_pa.h"

#include "interpret.h"

typedef struct f_frame{
    tItemList *return_Ins;
    Table_item *return_value;
    Table_item *next;
    struct f_frame *next_frame;
} frame;
//frame * ptr_stack;

// hlavicka pro interpret
int inter(tInstructionList *instrList);

#endif
