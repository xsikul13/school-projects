/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: ial.h
* Autor: Mour Lukáš, Motlík Matúš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "str.h"
#include "errors.h"

// Pro SORT
 #ifndef SORT
void fSORT(char *Item);
void QuickSort(char pole[], int left, int right);
#endif

// Pro FIND
#ifndef FIND
int fFIND(char *String, char *SubString);
void ComputeCharJump(char *SubString, int SubStringLenght, int *CharJump);
int BoyerMoore(char *String, char *SubString);
#endif

// Pro HASH TABLE
#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#include <stdbool.h>
#include "str.h"
#include "ilist.h"

#define TABLE_SIZE 193

// typedef enum {
//   UNKNOWN, TYP_INTEGER, TYP_REAL, TYP_STRING, TYP_BOOLEAN 
// } eType;

/**
 *  Unia pre ukladanie hodnot premennych roznych typov
 */
typedef union u_value {
  int typ_integer;
  double typ_real;
  string typ_string;
  bool typ_boolean;
} uValue;

/**
 * Struktura pre polozku tabulky typu premenna
 *  var_type - typ premennej
 *  var_value - hodnota premennej
 */
typedef struct {
  // eType var_type;
  int var_type;
  int local;
  uValue *var_value; 
} Table_item_var;

/**
 *  Struktura pre ulozenie ukazatelov na parametre funkcie
 *    param - ukazatel na parameter
 *    next_param - ukazatel na dalsi parameter
 */
typedef struct t_param{
  struct t_item *param;
  struct t_param *next_param;
} Par_names;

/**
 * Struktura pre polozku tabulky typu funkcia
 * local_ST - ukazatel na lokalnu TS
 * func_type - navratovy typ premennej
 * 
 *    doplnit dalsie polozky pre parametre, typ funkcie a pod.
 */
typedef struct {
  struct s_table* local_ST;
  // odkaz na prvu instrukciu funkcie
  void *instr;
  // eType func_type
  int func_type;
  bool forvard;
  // typy a pocet parametrov
  string* params;
  // nazvy parametrov
  Par_names* param_names;
} Table_item_func;

/**
 *  Rozlisenie typu polozky
 */
typedef enum {
  type_variable,
  type_function,
  DEFINE
} eType_of_item;

/**
 *  Unia, pre ukladanie premennej alebo funkcie do TS
 */
typedef union {
  Table_item_var var;
  Table_item_func func;
} uData;

/**
 * Struktura pre polozku tabulky symbolov
 * key  - struktura string, kde sa uklada atribut
 * type - typ polozky 
 * data - ukazatel na uniu
 * next - ukazatel na dalsiu polozku
 */
typedef struct t_item {
  string *key;
  eType_of_item type;
  uData *data;
  struct t_item *next;
} Table_item;

/**
 * Struktura pre tabulku symbolov
 *  size - velkost hash tabulky
 *  ptr  - pole ukazatelov na polozky tabulky
 */
typedef struct s_table {
  int size;
  Table_item *ptr[];
} Symbol_table;


/**
 * Prototypy funkcii
 */
Symbol_table *ST_init(int size); 
void ST_clear(Symbol_table *t);
void ST_free(Symbol_table *t);
Table_item* ST_find(Symbol_table *t, string *key);
int ST_save(Symbol_table *t, string *key, eType_of_item item_type, int type);
void ST_remove(Symbol_table *t, const char *key);
void ST_statistics(Symbol_table *t);
void ST_foreach(Symbol_table *t, void (*function)(const char*, int, int));
void print(const char* key, int type_of_item, int type);
unsigned int hash_function(const char *str, unsigned htab_size);

#endif
