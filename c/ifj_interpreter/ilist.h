/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: ilist.h
* Autor: Mour Lukáš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#ifndef ILIST_H
#define ILIST_H

#include <stdio.h>
#include <stdlib.h>
//#include "errors.h"
// Triadresny kod instrukce

enum instructionSet
{
    //genreicke instrukce
    I_STOP = 0,
    I_LAB,
    I_NOT,
    I_GOTO,
    I_IF_GOTO,

    // instrukcie pri volani fce
    I_CREATE_FRAME,
    I_RET_VALUE,
    I_CALL,
    I_CREATE_VAR,
    I_CREATE_PARAM,
    I_RETURN,                   //10
    I_CREATE_RETURN,

    I_PODMINKA_CYKLU,
    I_ZACATEK_CYKLU,
    I_KONEC_CYKLU,

    //instrukce cteni
    I_READLN,
    I_PRIRAD_REAL,
    I_READLN_INT,     
    I_READLN_STRING,
    I_READLN_REAL,

    //instrukce vypisu
    I_WRITE_INT,                //20
    I_WRITE_REAL,
    I_WRITE_STRING,
    I_WRITE_BOOL,

    //instrukce konverze typu - prirazeni hodnoty z addr1 (typ a) do addr2 (typ b)
    I_INT_TO_DOUBLE,
    I_DOUBLE_TO_INT,

    //INTEGER
    //instrukce pro pocitani s integery
    I_PRIRAD_INT,
    I_PLUS_INT,        
    I_MINUS_INT,
    I_KRAT_INT,
    I_DELENO_INT,                   //30
    I_NOT_INT,                  

    //instrukce pro rozhodovani o integerech
    I_ROVNO_INT,
    I_VETSI_INT,
    I_MENSI_INT,
    I_VETSIROVNO_INT,
    I_MENSIROVNO_INT,
    I_NEROVNO_INT,     

    //DOUBLE
    //instrukce pro pocitani s doubly
    I_PRIRAD_DOUBLE,
    I_PLUS_DOUBLE,
    I_MINUS_DOUBLE,                 //40
    I_KRAT_DOUBLE,              
    I_DELENO_DOUBLE,

    //instrukce pro rozhodovani o doublech
    I_ROVNO_DOUBLE,
    I_VETSI_DOUBLE,
    I_MENSI_DOUBLE,
    I_VETSIROVNO_DOUBLE,
    I_MENSIROVNO_DOUBLE,     
    I_NEROVNO_DOUBLE,

    //STRING
    //instrukce pro prirazeni (kopii) retezce
    I_PRIRAD_STRING,

    //instrukce pro rozhodovani o retezcich
    I_ROVNO_STRING,                 //50
    I_VETSI_STRING,
    I_MENSI_STRING,
    I_VETSIROVNO_STRING,
    I_MENSIROVNO_STRING,
    I_NEROVNO_STRING,

   // instrukcie na porovnavanie booleanu
    I_PRIRAD_BOOLEAN,
    I_ROVNO_BOOLEAN,
    I_VETSI_BOOLEAN,     
    I_MENSI_BOOLEAN,
    I_VETSIROVNO_BOOLEAN,               //60
    I_MENSIROVNO_BOOLEAN,
    I_NEROVNO_BOOLEAN,

    //instrukce konkatenace stringu
    I_SPOJIT_STRING,

    //instrukce serazeni retezce
    I_SORT_STRING,

    //instrukce vyhledani podretezce
    I_FIND_STRING,
    I_LENGTH,
    I_COPY,
    I_COPY_POM      // pomocna instrukcia na predanie vsetkych parametrov pre copy()
};

typedef struct {
int instType;
void *addr1;
void *addr2;
void *addr3;
} tInst;

// Struktura polozky seznamu

typedef struct ItemList {
tInst Data;
struct ItemList *NextItem;
} tItemList;

// Struktura pro seznam

typedef struct {
tItemList *First;
tItemList *Last;
tItemList *Active;
} tInstructionList;

void ListInit (tInstructionList *L);
void InsertLast (tInstructionList *L, tInst NewItem);
void ListDispose (tInstructionList *L);
void FirstActive (tInstructionList *L);
void NextActive (tInstructionList *L);
int Active(tInstructionList *L);
void TargetActive (tInstructionList *L, void *Target);
tInst *CurrentActive (tInstructionList *L);
void print_list(tInstructionList *L);

#endif

