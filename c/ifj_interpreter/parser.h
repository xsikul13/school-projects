/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: parser.h
* Autor: Motlík Matúš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#ifndef PARSER_H
#define PARSER_H

//#include "hash_table.h"
#include "ial.h"
#include "ilist.h"
#include "str.h"

void *main_instr;

// Makro pre ziskanie nasledujuceho tokenu a overenie ci sa zhoduje s rule
#define NEXT_TOKEN(token, rule) \
          if (( getNextToken(token) ) != (OK)) {\
            return (LEX_ERROR); }\
          if (token->stav != rule) { \
            if (rule == ID) { \
              switch (token->stav) { \
                case FIND: \
                case SORT: \
                case LENGTH: \
                case COPY: \
                  return SEM_ERROR_FUNCT_VAR; \
                  break; \
                } \
            } \
            return (SYNTAX_ERROR); \
          }

#define COPY_ATTR(attr) \
          string *(attr) = malloc(sizeof(string)); \
          if ((attr) == NULL) { \
            fprintf(stderr, "Malloc error\n"); \
            return RUNTIME_ERROR_OTHERS; \
          } \
          if (strInit((attr)) == STR_ERROR) \
            return RUNTIME_ERROR_OTHERS; \
          if (strCopyString((attr), token->atribut->t_STRING) == STR_ERROR) \
            return RUNTIME_ERROR_OTHERS

#define SAVE_TYPE_OF_FUNCTION item->data->func.func_type = token->stav; \
          Table_item *return_item; \
          return_item = ST_find(local_ST, attr); \
          return_item->data->var.var_type = token->stav
          
#define FIND_IN_TABLE(item) \
          if (((item) = ST_find(table, token->atribut->t_STRING)) == NULL) { \
            if (table == Global_ST) \
              return SEM_ERROR_FUNCT_VAR; \
            else \
              if (((item) = ST_find(Global_ST, token->atribut->t_STRING)) == NULL) \
                return SEM_ERROR_FUNCT_VAR; \
          } \
          if ((item)->type == type_function) \
            return SEM_ERROR_FUNCT_VAR


#define NEW_VAR(pom_item, new_var_name, typ) \
          string *(new_var_name); \
          if (((new_var_name) = malloc(sizeof(string))) == NULL) { \
            fprintf(stderr, "Malloc error\n"); \
            return RUNTIME_ERROR_OTHERS; } \
          if (strInit((new_var_name)) == STR_ERROR) \
            return RUNTIME_ERROR_OTHERS; \
          generateVariable((new_var_name)); \
          result = ST_save(Global_ST, (new_var_name), type_variable, (typ)); \
          if (result != OK) \
            return result; \
          (pom_item) = ST_find(Global_ST, (new_var_name))


void generate_instruction(int inst_type, void *addr1, void *addr2, void *addr3);
int parse(Symbol_table *symbol_table, tInstructionList* instr_list);
int program();
int var(Symbol_table *table);
int typ();
int premenne();
int decllist(Symbol_table *table);
int begin(Symbol_table *table);
int function();
int param_list(Symbol_table *local_ST, Table_item *fce_item);
int param_list2(Symbol_table *local_ST, Table_item *fce_item);
int funkcia(Symbol_table *local_ST, Table_item *item);
int decllist2(Symbol_table *local_ST, Table_item *item);
int telo(Symbol_table *table);
int prikaz(Symbol_table *table);
int param(Symbol_table *local_ST, Table_item *fce_item);
int _param(Symbol_table *local_ST);
int param2(Symbol_table *local_ST);
int zloz_prikaz(Symbol_table *table);
int readln(Symbol_table *table);
int id(Symbol_table *table);
int if_else(Symbol_table *table);
int while_do(Symbol_table *table);
int write(Symbol_table *table);
int _write(Symbol_table *table);
int term_list(Symbol_table *table);
int find(Symbol_table *table, Table_item *item);
int find2(Symbol_table *table, Table_item *s1, Table_item *item);
int sort(Symbol_table *table, Table_item *item);
int copy(Symbol_table *table, Table_item *item);
int copy2(Symbol_table *table, Table_item **item);
int length(Symbol_table *table, Table_item *item);
int repeat_until(Symbol_table *table);
int _telo(Symbol_table *table, int prev_token);

#endif
