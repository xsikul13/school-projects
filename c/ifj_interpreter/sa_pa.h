/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: sa_pa.h
* Autor: Marcin Juraj
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#ifndef SA_PA_H
#define SA_PA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "scaner.h"
#include "ilist.h"
#include "errors.h"
#include "parser.h"
#include "ial.h"

extern int counterVar;

/* Makra pre vytvorenie pomocnych premenny a ulozenie do tabuliek symbolov */

/* Makro CREATE_ITEM sluzi na vytvorenie a ulozenie polozky typu STRING do tabulky symbolov  */
 #define CREATE_ITEM(typ) \
    int result; \
    string *newVar; \
    if ((newVar = malloc(sizeof(string))) == NULL) { \
      fprintf(stderr, "Malloc error\n"); \
      return RUNTIME_ERROR_OTHERS; \
    } \
    if (strInit(newVar) == STR_ERROR)  \
      return RUNTIME_ERROR_OTHERS;  \
    generateVariable(newVar); \
    result = ST_save(Global_ST, newVar, type_variable, token->stav); \
    if (result != OK)  \
      return result; \
    item = ST_find(Global_ST, newVar); \
    item->data->var.var_type = (typ); \
      if (strInit(&(item->data->var.var_value->typ_string)) == STR_ERROR)  \
      return RUNTIME_ERROR_OTHERS;  \
      if (( strCopyString(&(item->data->var.var_value->typ_string), token->atribut->t_STRING)) == 1){ \
       fprintf(stderr, "Malloc error\n"); \
       return RUNTIME_ERROR_OTHERS; }
  
/* Makro CREATE_ITEM_OTHER sluzi na ulozenie polozky INEHO typu ako STRING
   teda NEMUSI alokovat miesto pre celu strukturu string */
  #define CREATE_ITEM_OTHER(typ) \
    int result; \
    string *newVar; \
    if ((newVar = malloc(sizeof(string))) == NULL) { \
      fprintf(stderr, "Malloc error\n"); \
      return RUNTIME_ERROR_OTHERS; \
    } \
    if (strInit(newVar) == STR_ERROR)  \
      return RUNTIME_ERROR_OTHERS;  \
    generateVariable(newVar); \
    result = ST_save(Global_ST, newVar, type_variable, token->stav); \
    if (result != OK)  \
      return result; \
    item = ST_find(Global_ST, newVar); \
    item->data->var.var_type = (typ)

/**************************************************************************/
/* Makro CREATE_ITEM_2 sluzi na vytvorenie a ulozenie pomocnej polozky
   typu STRING do tabulky symbolov. Pouziva sa pri konkatenacii retazcov */  
  #define CREATE_ITEM_2(typ) \
    int result1; \
    string *newVar1; \
    if ((newVar1 = malloc(sizeof(string))) == NULL) { \
      fprintf(stderr, "Malloc error\n"); \
      return RUNTIME_ERROR_OTHERS; \
    } \
    if (strInit(newVar1) == STR_ERROR)  \
      return RUNTIME_ERROR_OTHERS;  \
    generateVariable(newVar1); \
    result1 = ST_save(Global_ST, newVar1, type_variable, ID); \
    if (result1 != OK)  \
      return result1; \
    pom_item = ST_find(Global_ST, newVar1); \
    pom_item->data->var.var_type = (typ)

/* Makro CREATE_ITEM_OTHER_1 sluzi na vytvorenie a ulozenie pomocnej premennej
   INEHO typu ako STRING teda NEMUSI alokovat miesto pre celu strukturu string. */
      #define CREATE_ITEM_OTHER_1(typ) \
    int result1; \
    string *newVar1; \
    if ((newVar1 = malloc(sizeof(string))) == NULL) { \
      fprintf(stderr, "Malloc error\n"); \
      return RUNTIME_ERROR_OTHERS; \
    } \
    if (strInit(newVar1) == STR_ERROR)  \
      return RUNTIME_ERROR_OTHERS;  \
    generateVariable(newVar1); \
    result1 = ST_save(Global_ST, newVar1, type_variable, ID); \
    if (result1 != OK)  \
      return result1; \
    pom_item = ST_find(Global_ST, newVar1); \
    pom_item->data->var.var_type = (typ)

/*************************************************************************************/

  /* Makro CREATE_ITEM_OTHER_2 sluzi na vytvorenie a ulozenie pomocnej premennej
   INEHO typu ako STRING. Pouziva sa pre vytvorenie poloziek pre medzivysledky. */
   #define CREATE_ITEM_OTHER_2(typ) \
     int result2; \
    string *newVar2; \
    if ((newVar2 = malloc(sizeof(string))) == NULL) { \
      fprintf(stderr, "Malloc error\n"); \
      return RUNTIME_ERROR_OTHERS; \
    } \
    if (strInit(newVar2) == STR_ERROR)  \
      return RUNTIME_ERROR_OTHERS;  \
    generateVariable(newVar2); \
    result2 = ST_save(Global_ST, newVar2, type_variable, ID); \
    if (result2 != OK)  \
      return result2; \
    medzivysledok = ST_find(Global_ST, newVar2); \
    medzivysledok->data->var.var_type = (typ) 

/* Makro ktore zrusi zasobnik a ukonci pracu s listom instrukcii*/

#define DISPOSE_ALL() \
  Dispose(&zasobnik); 
 
/* Makro na zistenie ci je zasobnik vyprazdneny aby bolo jasne,
   ci teba skoncit alebo pokracovat vo vykonacani PA */

#define STACK_IN_END_STATE  zasobnik != NULL && zasobnik->hodnota == expression_se && zasobnik->right != NULL && zasobnik->right->hodnota == END_OF_FILE

/*Makro, ktre urci ci je dany stac konecny alebo ci sa ma pokracovat vo vykonavani PA*/

#define KONECNY_STAV() \
  if (STACK_IN_END_STATE && a == dolar_pa) \
  { \
    result = &aktualniToken;  \
        item = ST_find(table, aktualniToken.atribut->t_STRING) ; \
        *E = item;  \
    DISPOSE_ALL(); \
    return OK; \
  } 

enum e_symbol_PA
{
	// hodnota chyba
	error_pa = -1,
	// indexy do tabulky PA 
	left_bracket_pa = 0,
	right_bracket_pa,
	plus_minus_pa,
	multiply_divide_pa,
	compare_operator_pa,
	identificator_pa,
	dolar_pa,                 // 6
	// hodnoty v tabulke PA
	equal_pa,                 // 7
	lesser_pa,                // 8
	greater_pa,               // 9
	// pocet prvku
	count_pa
};

enum e_symbol_stack_extension
{   
    TO_COUNT =  91, 
	lesser_se = TO_COUNT,
	expression_se,
	expression_list_se,
	operator_se
};
/* struktura pre jednotlive polozky, s ktorymi 
   sa bude na zasobniku pracovat. */
typedef struct st_list_item
{
	Table_item *value;
	int hodnota;
	struct st_list_item* left;
	struct st_list_item* right;
} list_item;



int precedencna_SA(Symbol_table *Gtable,Symbol_table *Ltable,tInstructionList *list_instr, t_token *token,Table_item **E);
void generateVariable(string *var);
void generate_instruction(int inst_type, void *addr1, void *addr2, void *addr3);
int create_item (Table_item* Titem, Symbol_table *table, int stav,int typ);
int strCopyString_2(string s1, string *s2);
int strInit_2(string s);

#endif
