/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: main.c
* Autor: Motlík Matúš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#include <stdio.h>
#include "str.h"
#include "ial.h"
#include "scaner.h"
#include "parser.h"
#include "errors.h"
#include "ilist.h"
#include "sa_pa.h"
#include "interpret.h"


/**
 *  Hlavny program
 */
int main (int argc, char** argv) {
  FILE *f;
  if (argc != 2) {
    fprintf(stderr, "Chybny pocet parametrov programu!\n");
    return 99;
  }

  if ((f = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Soubor se nepodarilo otevrit!\n");
    return 99;
  }

  setSourceFile(f);

  // Inicializovanie tabulky symbolov ///////////osetrit chybove stavy
  Symbol_table *symbol_table = ST_init(TABLE_SIZE);
  // Inicializovanie tabulky instrukcii
  tInstructionList instr_list;
  ListInit(&instr_list);

  // Syntakticka analyza
  int result = parse(symbol_table, &instr_list);

  // vypisanie chybovych hlasok
  error(result);

  if (result != OK) {
    ST_free(symbol_table);
    ListDispose (&instr_list);
    fclose(f);
    return result;
  }

  // Interpretacia
	
  int interpret_out = inter(&instr_list);
  // vypisanie chybovych hlasok
  error(interpret_out);

 // printf("\ninterpret_out == %d\n", interpret_out);
  /* zkouška


#ifdef DEBUG
//  printf("\ninterpret_out == %d\n", interpret_out);

  instr_list.Active = instr_list.First;
  while (instr_list.Active != NULL){
    printf("prvni instrukce je %d addr1 %d \t addr2 %d \n",
      instr_list.Active->Data.instType, 
      ((Table_item*) instr_list.Active->Data.addr1)->data->var.var_value->typ_integer,
      ((Table_item*) instr_list.Active->Data.addr2)->data->var.var_value->typ_integer);
      instr_list.Active = instr_list.Active->NextItem;
  }
#endif
*/
  ListDispose(&instr_list);
  ST_free(symbol_table);
  fclose(f);

  return interpret_out;
}
