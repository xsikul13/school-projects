/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: parser.c
* Autor: Motlík Matúš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "str.h"
#include "scaner.h"
#include "ial.h"
#include "errors.h"
#include "ilist.h"
#include "sa_pa.h"

t_token *token;                 // globalna promenna, v ktorej je ulozeny aktualny token
Symbol_table *Global_ST;        // globalna premenna pre ulozenie tabulky symbolov
tInstructionList* list;         // globalna premenna pre ulozenie zoznamu instrukcii
int forward_count;              // premenna pre pocitanie doprednych deklaracii funkcii
string params;
string fce_id;                  // id aktualne spracovavanej funkcie
Par_names *param_name;          // kontrola parametrov


/**
 *  Vlozi novu instrukciu do zoznamu instrukcii
 */
void generate_instruction(int inst_type, void *addr1, void *addr2, void *addr3) {
  tInst I;
  I.instType = inst_type;
  I.addr1 = addr1;
  I.addr2 = addr2;
  I.addr3 = addr3;
  InsertLast (list, I);
}

/**
 *  Parse
 */
int parse(Symbol_table *symbol_table, tInstructionList* instr_list) {
  int result;
  Global_ST = symbol_table;
  list = instr_list;

  if ((token = token_alloc()) == NULL) {
    return RUNTIME_ERROR_OTHERS;
  }

  if (strInit(&params) != STR_SUCCESS)
    return RUNTIME_ERROR_OTHERS;

  // pociatocny stav struktury token
  token->stav = -1;
  if (getNextToken(token) != OK) {
    // chyba pri prvom tokene
    result = LEX_ERROR;
  }
  else {
    result = program();
  }

  // pridaj instrukciu pre koniec programu
  generate_instruction(I_STOP, NULL, NULL, NULL);

#ifdef DEBUG
  printf("\nGLOBALNA tabulka symbolov:(42-boolean, 11-integer, 13-real, 15-string)\n");
  printf("Nazov\t\t(0-var,1-func)\tTyp\n");
  ST_foreach(Global_ST, print); 
  printf("Koniec tabulky symbolov\n\n");
  // vypise zoznam instrukcii
  printf("Zoznam instrukcii:\n");
  print_list(list);
#endif

#ifdef STATISTICS 
  printf("Statistika globalnej tabulky:");
  ST_statistics(Global_ST);
#endif

  if(forward_count && result == OK) {
    fprintf(stderr, "Chyba definicia funkcie\n");
    return SEM_ERROR_FUNCT_VAR;
  }

  token_free(token);
  strFree(&params);

  return result;
}

/**
 *  Program na zaklade tokenu rozhodne ktore pravidlo sa bude vykonavat
 */
int program() {
  int result = SYNTAX_ERROR;

  switch (token->stav) {
    // pravidlo <parse> -> var id:<typ> ; <premenne> EOF
    case VAR:
      if ((result = var(NULL)) != OK)
        return result;

      // <premenne>
      result = premenne();
      if (result != OK)
        return result;
      
      // EOF
      if (token->stav != END_OF_FILE)
        return SYNTAX_ERROR;

      return OK;
      break;

    case FUNCTION:
      // <parse> -> function idF ( <param_list> :<typ>; <funkcia> <decllist> EOF
      if ((result = function(NULL)) != OK)
        return result;

      // EOF
      if (token->stav != END_OF_FILE)
        return SYNTAX_ERROR;

      return OK;
      break;

    case BEGIN:
      // <parse> -> begin <telo> end. EOF
      if ((result = begin(NULL)) != OK)
        return result;

      // EOF
      if (token->stav != END_OF_FILE)
        return SYNTAX_ERROR;

      return OK;
      break;
  }

  return SYNTAX_ERROR;
}

/**
 *  Funkcia na spracovanie pravidla
 *    id : <typ> ; 
 */
int var(Symbol_table *table) {
  // rozlisenie, ci je k dispozicii lokalna ST alebo len globalna
  if(table == NULL) 
    table = Global_ST;

  int result;

  // ID
  NEXT_TOKEN(token, ID);
  
  // ulozenie nazvu premennej pre neskorsie pouzitie
  COPY_ATTR(attr);

  // :
  NEXT_TOKEN(token, COLON);

  // <typ>
  result = typ();
  if (result != OK)
    return result;

  // ulozenie do tabulky symbolov
  result = ST_save(table, attr, type_variable, token->stav);
  if (result != OK)
    return result;

  if (table != Global_ST) {
    Table_item *i = ST_find(table, attr);
    // Instrukcia pre vytvorenie premennej na zasobniku
    generate_instruction(I_CREATE_VAR, i, NULL, NULL);
  }

  // ;
  NEXT_TOKEN(token, SEMICOLON);

  return OK;
}

/**
 *  Funkcia pre spracovanie typov premennych
 *    <typ> -> integer
 *    <typ> -> real
 *    <typ> -> string
 *    <typ> -> boolean
 */
int typ() {
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;  

  switch(token->stav) {
    case INTEGER:
      return OK;
    case REAL:
      return OK;
    case BOOLEAN:
      return OK;
    case STRING:
      return OK;
  }

  return SYNTAX_ERROR;
}

/**
 *  Funckia pre spracovanie dalsich premennych pri deklaracii
 *    spracovanie pravidiel  <premenne> -> id:<typ> ; <premenne>
 *                          <premenne> -> <dekllist> 
 */
int premenne() {
  int result;

  // ID
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  // <premenne> -> <decllist>   
  if (token->stav != ID) {
    return decllist(NULL);
  }
  //<premenne> -> id:<typ> ; <premenne>
  else { 
    // ulozenie nazvu premennej pre neskorsie pouzitie
    COPY_ATTR(attr);

    // :
    NEXT_TOKEN(token, COLON);

    // <typ>
    result = typ();
    if (result != OK)
      return result;

    // ulozenie do tabulky symbolov
    result = ST_save(Global_ST, attr, type_variable, token->stav);
    if (result != OK)
      return result;

    // ;
    NEXT_TOKEN(token, SEMICOLON);

    return premenne();
  }

  return SYNTAX_ERROR;
}

/**
 *  Funckia pre spracovanie pravidla 
 *       <decllist> -> begin <telo> end.
 *       <decllist> -> function idF ( <param_list> :<typ>; <funkcia> <decllist>
 */
int decllist(Symbol_table *table) {
  switch (token->stav) {
    // <decllist> -> begin <telo> end. 
    case BEGIN:
      return begin(table);
      break;
    
    // <decllist> -> function idF ( <param_list> :<typ>; <funkcia> <decllist>
    case FUNCTION:
      return function();
  }

  return SYNTAX_ERROR;
}

/**
 *  Funkcia na spracovanie pravidiel <telo> end.
 */
int begin(Symbol_table *table) {
  int result;

  generate_instruction(I_LAB, NULL, NULL, NULL);
  main_instr = (void*) list->Last;

  // <telo>
  if ((result = telo(table)) != OK)
    return result;

  // end.
  if (token->stav != END_OF_PROGRAM)
    return SYNTAX_ERROR;

  // len sa poziada o dalsi token a vrati sa uspech
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  return OK;
}

/**
 *  Funkcia pre spracovanie pravidla 
 *        function idF ( <param_list> ) :<typ>; <funkcia> <decllist>
 */
int function() {
  int result;
  Table_item *item;
  Symbol_table *local_ST;

  // IDF
  NEXT_TOKEN(token, ID);
  
  COPY_ATTR(attr);
  // vyhladam polozku a testujem ci je forvard, redeklarovana alebo ok
  item = ST_find(Global_ST, attr);
  // naslo polozku 
  if (item != NULL) {
    free(attr->str);
    free(attr);
    attr = item->key;
    // ale nie je forvard = redefinica funkcie
    if (item->data->func.forvard == false) {
      return SEM_ERROR_FUNCT_VAR;
    }
    // odratam od poctu forwardnutych funkcii, lebo nasleduje telo danej funkcie
    forward_count--;
    
    // Instrukcia navestia pre funkciu, adresu instrukcie si ulozim 
    generate_instruction(I_LAB, NULL, NULL, NULL);
    item->data->func.instr = (void*) list->Last;

    Table_item *return_item = ST_find(item->data->func.local_ST, attr);
    // Instrukcia pre vytvorenie premennej na zasobniku
    generate_instruction(I_CREATE_RETURN, return_item, NULL, NULL);
  }
  // polozku nenaslo, treba vytvorit novu  
  else {
    // ulozenie do globalnej tabulky symbolov
    result = ST_save(Global_ST, attr, type_function, INTEGER);
    if (result != OK)
      return result;
    
    item = ST_find(Global_ST, attr);
    item->data->func.forvard = false;

    // Instrukcia navestia pre funkciu, adresu instrukcie si ulozim 
    generate_instruction(I_LAB, NULL, NULL, NULL);
    item->data->func.instr = (void*) list->Last;

    // ulozeni do lokalnej tabulky symbolov nazov funkcie, sluzi pre return, typ sa zmeni neskor
    COPY_ATTR(return_attr);
    result = ST_save(item->data->func.local_ST, return_attr, type_variable, INTEGER);
    if (result != OK)
      return result;

    Table_item *return_item = ST_find(item->data->func.local_ST, return_attr);
    // Instrukcia pre vytvorenie premennej na zasobniku
    generate_instruction(I_CREATE_RETURN, return_item, NULL, NULL);
  }
  // ulozim si jej lokalnu TS
  local_ST = item->data->func.local_ST;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // <param_list>
  result = param_list(local_ST, item);
  if (result != OK)
    return result;

  // )
  if (token->stav != RIGHT_BRACKET)
    return SYNTAX_ERROR;

  // :
  NEXT_TOKEN(token, COLON);

  // <typ>
  result = typ();
  if (result != OK) 
    return result;

  SAVE_TYPE_OF_FUNCTION;

  // ;
  NEXT_TOKEN(token, SEMICOLON);

  // <funkcia>
  if ((result = funkcia(local_ST, item)) != OK)
    return result;

// kontrolny vypis lokalnej tabulky symbolov
#ifdef DEBUG
  printf("\nLOKALNA tabulka symbolov funkcie %s:(42-boolean, 11-integer, 13-real, 15-string)\n", item->key->str);
  printf("Nazov\t\t(0-var,1-func)\tTyp\n");
  ST_foreach(local_ST, print); 
#endif

#ifdef STATISTICS 
  printf("Statistika lokalnej tabulky funkcie %s:", item->key->str);
  ST_statistics(local_ST);
#endif

  // <decllist>
  // poziada sa o dalsi token a vrati sa navratova hodnota z decllist()
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  return decllist(NULL);
}

/**
 *  Funkcia pre spracovanie parametrov fukncie
 *    <param_list> -> id:<typ> <param_list2>
 *    <param_list> -> Ԑ
 */
int param_list(Symbol_table *local_ST, Table_item *fce_item) {
  int result;
  Table_item *param_item;
  param_name = fce_item->data->func.param_names;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;
  
  // Ak token nie je ID len vrati uspech
  // <param_list> -> Ԑ
  if (token->stav != ID) {
    // kontrola spravneho poctu parametrov pri doprednej deklaracii funkcie
    if (fce_item->data->func.forvard == true) {
      if (param_name->param != NULL)
        return SEM_ERROR_ARITH_EXPR;
    } 
    return OK;
  }
  else {
    // ulozenie nazvu premennej pre neskorsie pouzitie
    COPY_ATTR(attr);

    // :
    NEXT_TOKEN(token, COLON);

    // <typ>
    if ((result = typ()) != OK)
      return result;

    // ak je funkcie forward, skontrolujem ci sa nazvy parametrov zhoduju
    if (fce_item->data->func.forvard == true) {
      if ((param_item = ST_find(local_ST, attr)) == NULL) {
        free(attr->str);
        free(attr);
        return SEM_ERROR_ARITH_EXPR;
      }
      free(attr->str);
      free(attr);

      // kontrola typov
      if (param_item->data->var.var_type != token->stav)
        return SEM_ERROR_ARITH_EXPR;
      // kontrola ci ide o prvy parameter
      param_name = fce_item->data->func.param_names;
      if (param_item != param_name->param)
        return SEM_ERROR_ARITH_EXPR;
    }
    else {
      // ulozenie do tabulky symbolov 
      result = ST_save(local_ST, attr, type_variable, token->stav);
      if (result != OK)
        return result;
    
      // pocitadlo paramtrov (params->length) zvysim o 1 a ulozim typ parametru (params->str)
      if (strAddChar(fce_item->data->func.params, token->stav) != STR_SUCCESS)
        return RUNTIME_ERROR_OTHERS;

      param_item = ST_find(local_ST, attr);
      fce_item->data->func.param_names->param = param_item;
      fce_item->data->func.param_names->next_param = NULL;
      param_name = fce_item->data->func.param_names;
    }

    // <param_list2>
    if ((result = param_list2(local_ST, fce_item)) != OK)
      return result;
  }

  return OK;
}

/**
 *  Funkcia pre spracovanie dalsich parametrov funkcie
 *  <param_list2> -> , id:<typ> <param_list2>
 *  <param_list2> -> Ԑ
 */
int param_list2(Symbol_table *local_ST, Table_item *fce_item) {
  int result;
  Table_item *param_item;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;
  
  // ,
  if (token->stav == SEMICOLON) {
    // ID
    NEXT_TOKEN(token, ID);

    // ulozenie nazvu premennej pre neskorsie pouzitie
    COPY_ATTR(attr);

    // :
    NEXT_TOKEN(token, COLON);

    // <typ>
    if ((result = typ()) != OK)
      return result;

    // ak je funkcie forward, skontrolujem ci sa nazvy parametrov zhoduju
    if (fce_item->data->func.forvard == true) {
      if ((param_item = ST_find(local_ST, attr)) == NULL) {
        free(attr->str);
        free(attr);
        return SEM_ERROR_ARITH_EXPR;
      }
      free(attr->str);
      free(attr);

      // kontrola typov
      if (param_item->data->var.var_type != token->stav)
        return SEM_ERROR_ARITH_EXPR;
      // kontrola ci ide o spravny parameter
      param_name = param_name->next_param;
      if (param_name == NULL || param_item != param_name->param)
        return SEM_ERROR_ARITH_EXPR;
    }
    else {
      // ulozenie do tabulky symbolov 
      result = ST_save(local_ST, attr, type_variable, token->stav);
      if (result != OK)
        return result;
    
      // pocitadlo paramtrov (params->length) zvysim o 1 a ulozim typ parametru (params->str)
      if (strAddChar(fce_item->data->func.params, token->stav) != STR_SUCCESS)
        return RUNTIME_ERROR_OTHERS;
      
      //ulozim nazov parametru
      param_item = ST_find(local_ST, attr);
      Par_names *pom_param_name = param_name;
      param_name = param_name->next_param;
      if ((param_name = malloc(sizeof(Par_names))) == NULL)
        return RUNTIME_ERROR_OTHERS;
      param_name->next_param = NULL;
      param_name->param = param_item;
      pom_param_name->next_param = param_name;
    }
    
    // <param_list2>
    if ((result = param_list2(local_ST, fce_item)) != OK)
      return result;
  }

  // kontrola spravneho poctu parametrov pri doprednej deklaracii funkcie
  if (fce_item->data->func.forvard == true) {
    if (param_name->next_param != NULL){
      return SEM_ERROR_ARITH_EXPR;
    }
  }
  // Ak token nie je COMMA len vrati uspech
  // <param_list> -> Ԑ
  return OK;
}

/**
 *   Funkcia pre spracovanie tela funkcie
 *    <funkcia> -> forvard ; /////////////////upravit nejako pre premenne aj nazov funkcie, ohladom tabuliek symbolov
 *    <funkcia> -> var id:<typ> ; <decllist2>
 *    <funkcia> -> begin <telo> end; 
 */
int funkcia(Symbol_table *local_ST, Table_item *item) {
  int result;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    // forvard
    case FORWARD:
      if (item->data->func.forvard)
        return SEM_ERROR_FUNCT_VAR;
      item->data->func.forvard = true;

      // pocet forwardnutych funkcii
      forward_count++;

      // ;
      NEXT_TOKEN(token, SEMICOLON);

      return OK;
      break;
    // var
    case VAR:
      item->data->func.forvard = false;
      
      // id:<typ> ;
      if ((result = var(local_ST)) != OK)
        return result;

      // <decllist2>
      return decllist2(local_ST, item);

      break;
    // begin
    case BEGIN:
      item->data->func.forvard = false;

      if ((result = telo(local_ST)) != OK)
        return result;

      // end
      if (token->stav != END)
        return SYNTAX_ERROR;

      // ;
      NEXT_TOKEN(token, SEMICOLON);

      // return z funkcie, 
      generate_instruction(I_RETURN, NULL, NULL, NULL);

      return OK;
      break;
    }

  return SYNTAX_ERROR;
}

/**
 *  Funkcia na spracovanie pravidiel
 *    <decllist2> -> id:<typ> ; <decllist2>
 *    <decllist2> -> begin <telo> end;
 */
int decllist2(Symbol_table *local_ST, Table_item *item) {
  int result;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  // begin
  if (token->stav == BEGIN) {
    // <telo>
    if ((result = telo(local_ST)) != OK)
      return result;

    // end
    if (token->stav != END)
      return SYNTAX_ERROR;

    // ;
    NEXT_TOKEN(token, SEMICOLON);

    // return z funkcie, 
    generate_instruction(I_RETURN, NULL, NULL, NULL);
  }
  // id:<typ> ; <decllist2>
  else if (token->stav == ID) {
    // ulozenie nazvu premennej pre neskorsie pouzitie
    COPY_ATTR(attr);

    // :
    NEXT_TOKEN(token, COLON);

    // <typ>
    result = typ();
    if (result != OK)
      return result;

    // ulozenie do tabulky symbolov
    result = ST_save(local_ST, attr, type_variable, token->stav);
    if (result != OK)
      return result;

    Table_item *i = ST_find(local_ST, attr);
    // Instrukcia pre vytvorenie premennej na zasobniku
    generate_instruction(I_CREATE_VAR, i, NULL, NULL);

    // ;
    NEXT_TOKEN(token, SEMICOLON);

    // <decllist2>
    if ((result = decllist2(local_ST, item)) != OK)
      return result;
  }

  return OK;
}

/**
 *  Funkcia pre spracovanie pravidla <telo> -> <prikaz> <telo3>
 *                                   <telo> -> Ԑ
 */
int telo(Symbol_table *table) {
  int result;
  int prev_token = token->stav;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  // <telo> -> <prikaz> <telo3>
  switch(token->stav) {
    case WHILE:
    case BEGIN:
    case WRITE:
    case READLN:
      if ((result = prikaz(table)) != OK)
        return result;
      
      // osetrenie strednika za kazdym prikazom, za poslednym strednik nie je
      // <telo3> -> ; <telo>
      // <telo3> -> Ԑ
      if ((getNextToken(token)) != OK) 
        return LEX_ERROR;
      if (token->stav == SEMICOLON)
        return telo(table);
      // Ԑ
      return OK;
      break;
    case IF:
    case REPEAT:
      if ((result = prikaz(table)) != OK)
        return result;

      if (token->stav == SEMICOLON)
        return telo(table);
      // Ԑ
      return OK;
      break;
    case ID:
      if ((result = prikaz(table)) != OK)
        return result;
      
      // osetrenie strednika za kazdym prikazom, za poslednym strednik nie je
      // <telo3> -> ; <telo>
      // <telo3> -> Ԑ
      if (token->stav == SEMICOLON)
        return telo(table);
      // Ԑ
      return OK;
      break;

    // osetrenie, ze pred end a end. nemoze byt strednik
    case END:
    case END_OF_PROGRAM:
      if (prev_token == SEMICOLON)
        return SYNTAX_ERROR; 
      break;
  }

  // <telo> -> Ԑ
  return OK;
}

/**
 *  Funkcia pre spracovanie pravidiel:
 *    <prikaz> -> while PA do <zloz_prikaz>                      
 *    <prikaz< -> if PA then <zloz_prikaz> else <zloz_prikaz>  
 *    <prikaz> -> begin <telo> end
 *    <prikaz> -> id := <vyraz>        
 *    <prikaz> -> idF ( <param> )       
 *    <prikaz> -> PA                   
 *    <prikaz> -> readln (id) 
 *    <prikaz> -> write ( term <term_list> )
 */
int prikaz(Symbol_table *table) {
  int result;
  if (table == NULL)
    table = Global_ST;

  switch (token->stav) {
    // while PA do <zloz_prikaz>
    case WHILE:
      if ((result = while_do(table)) != OK)
        return result;
      break;

    case REPEAT:
      if ((result = repeat_until(table)) != OK)
        return result;
      break;

    // if PA then <zloz_prikaz> else <zloz_prikaz>  
    case IF:
      if ((result = if_else(table)) != OK)
        return result;
      break;

    // begin <telo> end
    case BEGIN:
      // begin
      if ((result = telo(table)) != OK)
        return result;

      // end
      if (token->stav != END)
        return SYNTAX_ERROR;
      break;

    // <prikaz> -> id := <vyraz>        
    case ID:
      if ((result = id(table)) != OK) 
        return result;
      break;

    // <prikaz> -> readln (id) 
    case READLN:
      if ((result = readln(table)) != OK)
        return result;
      break;

    // write ( "term" <term_list> )
    case WRITE:
      if ((result = write(table)) != OK)
        return result;
      break;

    // Vsetko ostatne je syntakticka chyba
    default:
      return SYNTAX_ERROR;
  }

  return OK;
}

/**
 *  Funkcia na spracovanie pravidiel:
 *    <param> -> id <param2>
 *    <param> -> Ԑ
 */
int param(Symbol_table *local_ST, Table_item *fce_item) {
  int result;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  strClear(&params);

  switch(token->stav) {
    case ID:
    case INTEGER_NUMB: 
    case DOUBLE_NUMB: 
    case C_STRING: 
    case TRUE_KW:
    case FALSE_KW: 
      param_name = fce_item->data->func.param_names;
      if ((result = _param(local_ST)) != OK)
        return result;
      break;
    default:
      return OK;
      break;
  }

  return param2(local_ST);
}

int _param(Symbol_table *local_ST) {
  Table_item *item;
  int result;

  // id <param>
  switch (token->stav) {
    case ID:
      // Ak ID nie je v tabulke symbolov
      if ((item = ST_find(local_ST, token->atribut->t_STRING)) == NULL)
        return SEM_ERROR_FUNCT_VAR;

      if (strAddChar(&params, item->data->var.var_type))
        return RUNTIME_ERROR_OTHERS;
      break;
    case INTEGER_NUMB: {
      // vytvorim polozku
      NEW_VAR(item, new_var_name, INTEGER);
      item->data->var.var_value->typ_integer = token->atribut->t_INTEGER;

      if (strAddChar(&params, INTEGER))
        return RUNTIME_ERROR_OTHERS;
      break;
    }
    case DOUBLE_NUMB: {
      // vytvorim polozku
      NEW_VAR(item, new_var_name, REAL);
      item->data->var.var_value->typ_real = token->atribut->t_REAL;

      if (strAddChar(&params, REAL))
        return RUNTIME_ERROR_OTHERS;
      break;
    }
    case C_STRING: {
      // vytvorim polozku
      NEW_VAR(item, new_var_name, STRING);
      strInit(&item->data->var.var_value->typ_string);
      strCopyString(&(item->data->var.var_value->typ_string), (token->atribut->t_STRING));
      
      if (strAddChar(&params, STRING))
        return RUNTIME_ERROR_OTHERS;
      break;
    }
    case TRUE_KW:
    case FALSE_KW: {
      // vytvorim polozku
      NEW_VAR(item, new_var_name, BOOLEAN);
      item->data->var.var_value->typ_boolean = (token->stav == TRUE_KW ? true : false);

      if (strAddChar(&params, BOOLEAN))
        return RUNTIME_ERROR_OTHERS;
      break;
    }
    default:
      // Ԑ
      break;
  }

  // Instrukcia pre vytvorenie polozky na zasobniku pre parameter
  if (param_name == NULL) {
    fprintf(stderr, "Too many parameters in function\n");
    return SEM_ERROR_ARITH_EXPR;
  }
  // prva polozka sluzi na odovzdanie nazvu parametru, 
  //    druha pre hodnotu ktoru ma mat parameter pri volani fce
  generate_instruction(I_CREATE_PARAM, param_name->param, item, NULL);

  return OK;
}

/**
 *  Funkcia na spracovanie pravidiel:
 *    <param2> -> , id <param2>
 *    <param2> -> Ԑ
 */
int param2(Symbol_table *local_ST) {
  int result;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  // Ԑ
  if (token->stav != COMMA) {
    return OK;
  }

  // , id <param>
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch(token->stav) {
    case ID:
    case INTEGER_NUMB: 
    case DOUBLE_NUMB: 
    case C_STRING: 
    case TRUE_KW:
    case FALSE_KW: 
      param_name = param_name->next_param;
      if ((result = _param(local_ST)) != OK)
        return result;
      break;
    default:
      return SYNTAX_ERROR;
      break;
  }

  return param2(local_ST);
}

/**
 * Funkcia na spracovanie pravidla:
 *    <zloz_prikaz> -> begin <telo> end
 */
int zloz_prikaz(Symbol_table *table) {
  int result;

  // begin
  NEXT_TOKEN(token, BEGIN);

  // <telo>
  if ((result = telo(table)) != OK)
    return result;

  // end
  if (token->stav != END)
    return SYNTAX_ERROR;

  return OK;
}

/**
 *  Funkcia na skontrolovanie syntaxe pre vyrazy a vygenerovanie instrukcii
 *      <prikaz> -> id := <vyraz>
 *          <vyraz> -> PA
 *          <vyraz> -> idF ( <param> )
 */
int id(Symbol_table *table) {
  int result;
  Table_item *item = NULL;
  Table_item *pom_item = NULL;
  Table_item *fce_item = NULL;

  // Najde ID v tabulke a do premennej item ulozi ukazatel na danu polozku v ST
  FIND_IN_TABLE(item);

  // :=
  NEXT_TOKEN(token, ASSIGN);

  // hlada sa ID funkcie
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;    

  // ak je to vstavana funkcia
  switch (token->stav) {
    // find( s : string; search : string) : integer
    // find( id, id)
    case FIND:
      if (item->data->var.var_type != INTEGER)
        return SEM_ERROR_ARITH_EXPR;

      if ((result = find(table, item)) != OK)
        return result;
      return OK;
      break;

    // sort( s : string) : string
    // sort (id)
    case SORT:
      if (item->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;

      if ((result = sort(table, item)) != OK)
        return result;
      return OK;
      break;

    // copy( s : string; i : integer; n : integer) : string
    // copy (id, id, id)
    case COPY:
      if (item->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;

      if ((result = copy(table, item)) != OK)
        return result;
      return OK;
      break;

    // length( s : string) : integer
    // length (id)
    case LENGTH:
      if (item->data->var.var_type != INTEGER)
        return SEM_ERROR_ARITH_EXPR;

      if ((result = length(table, item)) != OK)
        return result;
      return OK;
      break;

    // je to hodnota(int/real/string)
    case INTEGER_NUMB:
    case DOUBLE_NUMB:
    case TRUE_KW:
    case FALSE_KW:
    case C_STRING:
      break;

    default:
      // idF ( <param> )
      fce_item = ST_find(Global_ST, token->atribut->t_STRING);
      if (fce_item != NULL && fce_item->type == type_function) {
          // kontrola typov
          if (item->data->var.var_type != fce_item->data->func.func_type) {
            fprintf(stderr, "Incompatible type of variable\n");
            return SEM_ERROR_ARITH_EXPR;
          }

        // (
        NEXT_TOKEN(token, LEFT_BRACKET);

        // skontroluje pocet a typy parametrov
        if ((result = param(table, fce_item)) != OK)
          return result;
        if (strCmpString(fce_item->data->func.params, &params) != 0)
          return SEM_ERROR_ARITH_EXPR;

        // )
        if (token->stav != RIGHT_BRACKET)
          return SYNTAX_ERROR;

        // instrukcia pre volanie funkcie
        generate_instruction(I_CALL, fce_item, item, NULL);

        if ((getNextToken(token)) != OK) 
          return LEX_ERROR;
        return OK;
      }
      break;
    }

    // nie je to ani funkcia, tak volaj PA a vrat jej vysledok
    if ((result = precedencna_SA(Global_ST, table, list, token, &pom_item)) != OK)
      return result;
    switch(item->data->var.var_type) {
      case INTEGER:
        // nie su oba integer
        if (pom_item->data->var.var_type != INTEGER) {
          fprintf(stderr, "Incompatible type of variable\n");
          return SEM_ERROR_ARITH_EXPR;
        }
        generate_instruction(I_PRIRAD_INT, pom_item, NULL, item);
        break;

      case REAL:
        // nie su oba real
        if (pom_item->data->var.var_type != REAL) {
          fprintf(stderr, "Incompatible type of variable\n");
          return SEM_ERROR_ARITH_EXPR;
        }
        generate_instruction(I_PRIRAD_DOUBLE, pom_item, NULL, item);
        break;

      case STRING:
        // nie su oba string
        if (pom_item->data->var.var_type != STRING) {
          fprintf(stderr, "Incompatible type of variable\n");
          return SEM_ERROR_ARITH_EXPR;
        }
        generate_instruction(I_PRIRAD_STRING, pom_item, NULL, item);
        break;

      case BOOLEAN:
        if (pom_item->data->var.var_type != BOOLEAN) {
          fprintf(stderr, "Incompatible type of variable\n");
          return SEM_ERROR_ARITH_EXPR;
        }
        generate_instruction(I_PRIRAD_BOOLEAN, pom_item, NULL, item);
        break;
    }

  return OK;
}

/**
 * Funkcia na skontrolovanie syntaxe pre prikaz if-else a vygenerovanie instrukcii
 *    <prikaz< -> if PA then <zloz_prikaz> else <zloz_prikaz>
 */
int if_else(Symbol_table *table) {
  int result;
  tInst *data;
  void *addr_of_L1;
  void *addr_of_L2;
  void *addr_of_if_Goto;
  void *addr_of_Goto;

  //vygenerujeme pomocnu polozku v TS, v ktorej bude ulozeny "vysledok" PA
  Table_item *pom_item = NULL;

  // PA
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;
  if ((result = precedencna_SA(Global_ST, table, list, token, &pom_item)) != OK)
    return result;

  if (pom_item->data->var.var_type != BOOLEAN) {
    fprintf(stderr, "Incompatible type of variable!\n");
    return SEM_ERROR_ARITH_EXPR;
  }

  // then
  if (token->stav != THEN)
    return SYNTAX_ERROR;

  Table_item *pom_item_not;
  NEW_VAR(pom_item_not, new_var_name_not, pom_item->data->var.var_type);
  // znegujeme vysledok
  generate_instruction(I_NOT, pom_item, NULL, pom_item_not);
  // nagenerujeme instrukciu podmieneneho skoku a ulozime jej adresu,
  generate_instruction(I_IF_GOTO, (void*) pom_item_not, NULL, NULL); 
  addr_of_if_Goto = (void*) list->Last;

  //<zloz_prikaz>
  if ((result = zloz_prikaz(table)) != OK)
    return result;

  // priamy skok na navestie na koniec if-else
  generate_instruction(I_GOTO, NULL, NULL, NULL);
  addr_of_Goto = (void*) list->Last;

  // nagenerujeme instrukciu navestia pre else, ak else nie je pritomny, skace sa na koniec if
  generate_instruction(I_LAB, NULL, NULL, NULL);
  addr_of_L1 = (void*) list->Last;

  // else
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;
  
  // ak je pritomna vetva else
  if (token->stav == ELSE) {

    //<zloz_prikaz>
    if ((result = zloz_prikaz(table)) != OK)
      return result;

    // poziadam o dalsi token
    if ((getNextToken(token)) != OK) 
      return LEX_ERROR;

    // nagenerujeme instrukciu navestia pre koniec if-else
    generate_instruction(I_LAB, NULL, NULL, NULL);
    addr_of_L2 = (void*) list->Last;
    // pozname adresu navestia pre else, tak ju ulozime do nagenerovanej instrukcie podmieneneho skoku
    TargetActive (list, addr_of_Goto);
    data = CurrentActive(list);
    data->addr3 = addr_of_L2;
  }
  else {
    TargetActive (list, addr_of_Goto);
    data = CurrentActive(list);
    data->addr3 = addr_of_L1;
  }

  // pozname adresu navestia pre else, tak ju ulozime do nagenerovanej instrukcie podmieneneho skoku
  TargetActive (list, addr_of_if_Goto);
  data = CurrentActive(list);
  data->addr3 = addr_of_L1; 

  return OK;
}

/**
 *  Funkcia na skontrolovanie syntaxe pre prikaz while - do a vygenerovanie instrukcii
 *     while PA do <zloz_prikaz> 
 */
int while_do(Symbol_table *table) {
  //vygenerujeme pomocnu polozku v TS, v ktorej bude ulozeny "vysledok" PA
  Table_item *pom_item = NULL;
  // pomocna premenna pre znegovany vysledok
  Table_item *pom_item_not;
  int result;

  // vygenerujeme instrukciu noveho navestia a ulozime jej ukazatel
  generate_instruction(I_LAB, NULL, NULL, NULL);
  void *addrOfLab1;
  addrOfLab1 = (void*) list->Last;

  // PA
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  if ((result = precedencna_SA(Global_ST, table, list, token, &pom_item)) != OK)
    return result;
  if (pom_item->data->var.var_type != BOOLEAN) {
    fprintf(stderr, "Incompatible type of variable!\n");
    return SEM_ERROR_ARITH_EXPR;
  }
  
  NEW_VAR(pom_item_not, new_var_name_not, pom_item->data->var.var_type);
  // znegujeme vysledok
  generate_instruction(I_NOT, pom_item, NULL, pom_item_not);

  // nagenerujeme instrukciu podmieneneho skoku a ulozime jej adresu,
  void *addrOfIfGoto;
  generate_instruction(I_IF_GOTO, (void*) pom_item_not, NULL, NULL); 
  addrOfIfGoto = (void*) list->Last;

  // do
  if (token->stav != DO)
    return SYNTAX_ERROR;
      
  //<zloz_prikaz>
  if ((result = zloz_prikaz(table)) != OK)
    return result;

  // nagenerujeme instrukciu skoku
  generate_instruction(I_GOTO, NULL, NULL, (void*) addrOfLab1);

  // nagenerujeme instrukciu druheho navestia
  generate_instruction(I_LAB, NULL, NULL, NULL);
  void *addrOfLab2;
  addrOfLab2 = (void*) list->Last;

  // jiz zname adresu druheho navesti, muzeme tedy nastavit adresu
  // do drive generovane instrukce podmineneho skoku
  TargetActive (list, addrOfIfGoto);
  tInst *data;
  data = CurrentActive(list);
  data->addr3 = addrOfLab2;

  return OK;
}

/**
 *  Funkcia na skontrolovanie syntaxe pre readln a vygenerovanie instrukcii
 *      <prikaz> -> readln (id) 
 */
int readln(Symbol_table *table) {
  Table_item *item = NULL;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // ID
  NEXT_TOKEN(token, ID);
  // Ak ID nie je v tabulke symbolov
  FIND_IN_TABLE(item);

  // Ak je typ boolean alebo je to id funkcie je to sem. chyba
  if (item->type != type_variable || item->data->var.var_type == BOOLEAN)
    return SEM_ERROR_ARITH_EXPR;

  // )
  NEXT_TOKEN(token, RIGHT_BRACKET);

  // pridaj instrukciu pre readln
  switch (item->data->var.var_type) {
    case INTEGER:
      generate_instruction(I_READLN_INT, NULL, NULL, item);
      break;
    case REAL:
      generate_instruction(I_READLN_REAL, NULL, NULL, item);
      break;
    case STRING:
      generate_instruction(I_READLN_STRING, NULL, NULL, item);
      break;
    default:
      return SEM_ERROR_ARITH_EXPR;
  }

  return OK;
}

/**
 * Funkcia na skontrolovanie syntaxe pre prikaz write() a vygenerovanie instrukcii
 *      write ("term" <term_list> )
 *          <term_list> -> ,"term" <term_list>
 *          <term_list> -> Ԑ
 */
int write(Symbol_table *table) {
  int result;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // "term"
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  if ((result = _write(table)) != OK)
    return result;

  if ((result = term_list(table)) != OK)
    return result;

  // )
  if (token->stav != RIGHT_BRACKET)
    return SYNTAX_ERROR;

  return OK;
}

int _write(Symbol_table *table) {
  int result;
  Table_item *item = NULL;

  switch (token->stav) {
    case ID:
      // vyhladanie v TS
      FIND_IN_TABLE(item);
      // Akje to id funkcie je to sem. chyba
      if (item->type != type_variable)
        return SEM_ERROR_ARITH_EXPR;
        // pridaj instrukciu pre write
      switch (item->data->var.var_type) {
        case INTEGER:
          generate_instruction(I_WRITE_INT, NULL, NULL, item);
          break;
        case REAL:
          generate_instruction(I_WRITE_REAL, NULL, NULL, item);
          break;
        case STRING:
          generate_instruction(I_WRITE_STRING, NULL, NULL, item);
          break;
        case BOOLEAN:
          generate_instruction(I_WRITE_BOOL, NULL, NULL, item);
          break;
        default:
          return SEM_ERROR_ARITH_EXPR;
        }
      break;

    case INTEGER_NUMB: {
      // vytvorim polozku
      Table_item *pom_item;
      NEW_VAR(pom_item, new_var_name, INTEGER);
      
      pom_item->data->var.var_value->typ_integer = token->atribut->t_INTEGER;
      // generujem instrukciu pre vypis integeru
      generate_instruction(I_WRITE_INT, NULL, NULL, pom_item);
      break;
    }
    case DOUBLE_NUMB: {
      // vytvorim polozku
      Table_item *pom_item;
      NEW_VAR(pom_item, new_var_name, REAL);

      pom_item->data->var.var_value->typ_real = token->atribut->t_REAL;
      // generujem instrukciu pre vypis integeru
      generate_instruction(I_WRITE_REAL, NULL, NULL, pom_item);
      break;
    }
    case C_STRING: {
      // vytvorim polozku
      Table_item *pom_item;
      NEW_VAR(pom_item, new_var_name, STRING);
      // generujem instrukciu pre vypis integeru
      strInit(&pom_item->data->var.var_value->typ_string);
      strCopyString(&(pom_item->data->var.var_value->typ_string), (token->atribut->t_STRING));
      generate_instruction(I_WRITE_STRING, NULL, NULL, pom_item);
      break;
    }
    case FALSE_KW:
    case TRUE_KW: {
      // vytvorim polozku
      Table_item *pom_item;
      NEW_VAR(pom_item, new_var_name, BOOLEAN);

      pom_item->data->var.var_value->typ_boolean = (token->stav == TRUE_KW ? true : false);
      // generujem instrukciu pre vypis integeru
      generate_instruction(I_WRITE_BOOL, NULL, NULL, pom_item);
      break;
    }
    default:
      return SYNTAX_ERROR;
      break;
  }
  return OK;
}

/**
 * Funkcia pre pravidlo: <term_list> -> ,"term" <term_list>
 *                       <term_list> -> Ԑ
 */
int term_list(Symbol_table *table) {
  int result;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  // ,"term" <term_list>
  if (token->stav == COMMA) {
    if ((getNextToken(token)) != OK) 
      return LEX_ERROR;

    // "term"
    if ((result = _write(table)) != OK)
      return result;

    return term_list(table);
  }

  // Ԑ
  return OK;
}

/**
 *  find( s : string; search : string) : integer
 *      find( id, id)
 */
int find(Symbol_table *table, Table_item *item) {
  int result;
  Table_item *s1 = NULL;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // id alebo string
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    case ID: {
      // vyhladanie v TS
      FIND_IN_TABLE(s1);
      // Ak je to id funkcie je to sem. chyba
      if (s1->type != type_variable || s1->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;
      break;
    }
    case C_STRING: {
      // vytvorim polozku
      NEW_VAR(s1, new_var_name, STRING);
      strInit(&s1->data->var.var_value->typ_string);
      strCopyString(&(s1->data->var.var_value->typ_string), (token->atribut->t_STRING));
      break;
    }
    default:
      return SEM_ERROR_ARITH_EXPR;
      break;
  }

  if ((result = find2(table, s1, item)) != OK)
    return result;

  // )
  NEXT_TOKEN(token, RIGHT_BRACKET);

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  return OK;
}

/**
 *  Pomocna funkcia pre find()
 */
int find2(Symbol_table *table, Table_item *s1, Table_item *item) {
  int result;
  Table_item *s2 = NULL;

  // ,
  NEXT_TOKEN(token, COMMA);

  // id alebo string
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    case ID: {
      // vyhladanie v TS
      FIND_IN_TABLE(s2);
      // Ak je to id funkcie je to sem. chyba
      if (s2->type != type_variable || s2->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;      
      break;
    }

    case C_STRING: {
      // vytvorim polozku
      NEW_VAR(s2, new_var_name, STRING);
      strInit(&s2->data->var.var_value->typ_string);
      strCopyString(&(s2->data->var.var_value->typ_string), (token->atribut->t_STRING));
      break;
    }
    default:
      return SEM_ERROR_ARITH_EXPR;
      break;
  }

  // pridaj instrukciu pre length
  generate_instruction(I_FIND_STRING, s1, s2, item);

  return OK;
}

/**
 *  sort( s : string) : string
 *      sort (id)
 */
int sort(Symbol_table *table, Table_item *item) {
  int result;
  Table_item *s = NULL;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // id alebo string
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    case ID: {
      // vyhladanie v TS
      FIND_IN_TABLE(s);
      // Ak je to id funkcie je to sem. chyba
      if (s->type != type_variable || s->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;
      break;
    }
    case C_STRING: {
      // vytvorim polozku
      NEW_VAR(s, new_var_name, STRING);
      strInit(&s->data->var.var_value->typ_string);
      strCopyString(&(s->data->var.var_value->typ_string), (token->atribut->t_STRING));
      break;
    }
    default:
      return SEM_ERROR_ARITH_EXPR;
  }

  // pridaj instrukciu pre sort
  generate_instruction(I_SORT_STRING, s, NULL, item);

  // )
  NEXT_TOKEN(token, RIGHT_BRACKET);

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  return OK;
}

/**
 *  copy( s : string; i : integer; n : integer) : string
 *      copy (id, id, id)
 */
int copy(Symbol_table *table, Table_item *item) {
  int result;
  Table_item *s = NULL;
  Table_item *i = NULL;
  Table_item *n = NULL;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // id alebo string
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    case ID: {
      // vyhladanie v TS
      FIND_IN_TABLE(s);
      // Ak je to id funkcie je to sem. chyba
      if (s->type != type_variable || s->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;
      break;
    }

    case C_STRING: {
      // vytvorim polozku
      NEW_VAR(s, new_var_name, STRING);
      strInit(&s->data->var.var_value->typ_string);
      strCopyString(&(s->data->var.var_value->typ_string), (token->atribut->t_STRING));
      break;
    }
    default:
      return SEM_ERROR_ARITH_EXPR;
      break;
  }

  // pre i:integer
  if ((result = copy2(table, &i)) != OK)
    return result;
  // pre n:integer
  if ((result = copy2(table, &n)) != OK)
    return result;

  // pridaj pomocnu instrukciu pre copy
  generate_instruction(I_COPY_POM, i, n, NULL);
  // pridaj instrukciu pre copy
  generate_instruction(I_COPY, s, NULL, item);

  // )
  NEXT_TOKEN(token, RIGHT_BRACKET);

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  return OK;
}

/**
 *  Pomocna funkcia pre copy
 */
int copy2(Symbol_table *table, Table_item **item) {
  int result;

  // ,
  NEXT_TOKEN(token, COMMA);

  // id alebo integer
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    case ID: {
      // vyhladanie v TS
      FIND_IN_TABLE(*item);
      // Ak je to id funkcie je to sem. chyba
      if ((*item)->type != type_variable || (*item)->data->var.var_type != INTEGER)
        return SEM_ERROR_ARITH_EXPR;   
    
      break;
    }
    case INTEGER_NUMB: {
      // vytvorim polozku
      Table_item *i;
      NEW_VAR(i, new_var_name, INTEGER);
      i->data->var.var_value->typ_integer = token->atribut->t_INTEGER;
      *item = i;
      break;
    }
    default:
      return SEM_ERROR_ARITH_EXPR;
      break;
  }
  return OK;
}

/**
 *  length( s : string) : integer
 *      length (id)
 */
int length(Symbol_table *table, Table_item *item) {
  int result;
  Table_item *s = NULL;

  // (
  NEXT_TOKEN(token, LEFT_BRACKET);

  // id alebo string
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  switch (token->stav) {
    case ID: {
      // vyhladanie v TS
      FIND_IN_TABLE(s);
      // Ak je to id funkcie je to sem. chyba
      if (s->type != type_variable || s->data->var.var_type != STRING)
        return SEM_ERROR_ARITH_EXPR;
      break;
    }
    case C_STRING: {
      // vytvorim polozku
      NEW_VAR(s, new_var_name, STRING);
      strInit(&s->data->var.var_value->typ_string);
      strCopyString(&(s->data->var.var_value->typ_string), (token->atribut->t_STRING));
      break;
    }
    default:
      return SEM_ERROR_ARITH_EXPR;
  }

  // pridaj instrukciu pre length
  generate_instruction(I_LENGTH, s, NULL, item);

  // )
  NEXT_TOKEN(token, RIGHT_BRACKET);

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  return OK;
}

/**
 *  Funkcia na skontrolovanie syntaxe pre prikaz repeat-until a vygenerovanie instrukcii
 *     repeat <zloz_prikaz> until PA 
 */
int repeat_until(Symbol_table *table) {
  int result;
  //vygenerujeme pomocnu polozku v TS, v ktorej bude ulozeny "vysledok" PA
  Table_item *pom_item = NULL;
  // pomocna premenna pre znegovany vysledok
  Table_item *pom_item_not;

  // vygenerujeme instrukciu noveho navestia a ulozime jej ukazatel
  generate_instruction(I_LAB, NULL, NULL, NULL);
  void *addrOfLab1;
  addrOfLab1 = (void*) list->Last;


  if ((result = _telo(table, REPEAT)) != OK)
    return result;

  if (token->stav != UNTIL)
    return SYNTAX_ERROR;


  // PA
  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  if ((result = precedencna_SA(Global_ST, table, list, token, &pom_item)) != OK)
    return result;
  if (pom_item->data->var.var_type != BOOLEAN) {
    fprintf(stderr, "Incompatible type of variable!\n");
    return SEM_ERROR_ARITH_EXPR;
  }

  NEW_VAR(pom_item_not, new_var_name_not, pom_item->data->var.var_type);
  // znegujeme vysledok
  generate_instruction(I_NOT, pom_item, NULL, pom_item_not);

  // nagenerujeme instrukciu podmieneneho skoku
  generate_instruction(I_IF_GOTO, (void*) pom_item_not, NULL, addrOfLab1); 

  return OK;
}


int _telo(Symbol_table *table, int prev_token) {
  int result;

  if ((getNextToken(token)) != OK) 
    return LEX_ERROR;

  // <telo> -> <prikaz> <telo3>
  switch(token->stav) {
    case UNTIL:
      if (prev_token == REPEAT || prev_token == SEMICOLON)
        return SYNTAX_ERROR;
      break;
    case WHILE:
    case IF:
    case WRITE:
    case READLN:
      if ((result = prikaz(table)) != OK)
        return result;
      
      if ((getNextToken(token)) != OK) 
        return LEX_ERROR;
      if (token->stav == SEMICOLON)
        return _telo(table, SEMICOLON);
      break;
    case REPEAT:
    case ID:
      if ((result = prikaz(table)) != OK)
        return result;
      
      if (token->stav == SEMICOLON)
        return _telo(table, SEMICOLON);
      break;
    case BEGIN:
      if ((result = _telo(table, BEGIN)) != OK)
        return result;
      if (token->stav != END)
        return SYNTAX_ERROR;

      if ((getNextToken(token)) != OK) 
        return LEX_ERROR;
      if (token->stav == SEMICOLON)
        return _telo(table, SEMICOLON);
      break;
  }

  prev_token = token->stav;
  // <telo> -> Ԑ
  return OK;
}