/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: ial.c
* Autor: Mour Lukáš, Motlík Matúš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

#include "ial.h"
#include "scaner.h"

//***********************************************************
// Funkce pro VESTAVENOU FUNKCI SORT (podle IAL skript)

void fSORT(char *Item)
{
  // Treba znat delku pro vymezeni intervalu
  int lenght;
  lenght = (strlen(Item) - 1);

  // Volani fce pro razeni = QuickSort
  QuickSort(Item, 0, lenght);
}

// Implementace radiciho algoritmu QuickSort

// Rozdeleni pole prvku na 2 casti
void QuickSort(char *Item, int left, int right)
{
	int PM; // Promenna pseudomedianu
 	int i;
 	int j;

 	i = left; // Vymezeni intervalu
 	j = right;
 	PM = Item[(i + j) / 2]; // Stanoveni pseudommedianu
  
  char TempArray; // Pomocne pole pro zamenu 2 poli

 	while(i <= j) // Cyklus konci, jsou-li indexy prekrizeny
 	{
 		while(Item[i] < PM)
    {
 			i++;
    }
 		while(Item[j] > PM)
    {
 			j--;
    }
 		if(i <= j)
 		{
			
 			TempArray = Item[i]; // Vzajemna zamena
 			Item[i] = Item[j];
 			Item[j] = TempArray;
 			i++;
 			j--;
 		}
 	}

  // Rekurzivni volani Quicksortu
  if (left < j)
      QuickSort(Item, left, j);
  if (i < right)
      QuickSort(Item, i, right);
}

//***********************************************************

//***********************************************************
// Funkce pro VESTAVENOU FUNKCI FIND (podle IAL skript)

int fFIND(char *String, char *SubString)
{
  return BoyerMoore(String, SubString);
}

#define ARRAY_CORRECTION 2 // V Pascalu korekce 1 (C indexuje od 0, proto 2)
#define MAX_CHAR 256

// 1. HEURISTIKA BOYER MOOROVA ALGORITMU
void ComputeCharJump(char *SubString, int SubStringLenght, int *CharJump)
{
    for (int i = 0; i < MAX_CHAR; i++)
    {
        CharJump[i] = SubStringLenght;
    }

    for (int i = 0; i < SubStringLenght; i++)
    {
        CharJump[(int) SubString[i]] = SubStringLenght - (i + 1);
    }
}

int BoyerMoore(char *String, char *SubString)
{
    int StringLenght = strlen(String); // Delka retezce kde hledam
    int SubStringLenght = strlen(SubString); // Delka podretezce

    int CharJump[MAX_CHAR]; // Maximalni abeceda

    int MAX_COMPARES = 0; // Osetreni proti zacykleni
    int Pom = 0;

    int j = (SubStringLenght - 1); // Indexy
    int k = (SubStringLenght - 1);

    ComputeCharJump(SubString, SubStringLenght, CharJump); // Vypocet skoku

    if (StringLenght == 0) // Retezec v kterem vyhledavame je prazdny
    {
        return 0; // Nenalezeno
    }

    if(SubStringLenght > StringLenght) // Podretezec je vetsi nez retezec, kde hledam
    {
        return 0; // Nenalezeno
    }

    if (SubStringLenght == 0) // Vyhledavam podretezec nulove delky
    {
        return 1; // EXISTUJE VZDY na 1. pozici
    }
    
    while ((j < StringLenght) && (k >= 0))
    {
        MAX_COMPARES++; // Pocitadlo, kolikrat prosel cyklus

        // Osetreni case sensitive
        String[j] = tolower(String[j]);
        SubString[k] = tolower(SubString[k]);
        
        if (String[j] == SubString[k])
        {
            if(k > Pom && MAX_COMPARES > StringLenght)
            {
                j = j+2;
            }

            j--;
            Pom = k;
            k--;
        }
        else
        {
            j += CharJump[(int) String[j]];
            k = SubStringLenght - 1;
        }
    }
    if (k == -1) // Shoda nalezena
    {
       return (j + ARRAY_CORRECTION); // Treba provest korekci pozice ... VYSLEDEK
    }        
    else // Shoda nenalezena
    {
       return 0; 
    } 
}

//***********************************************************

//***********************************************************
// Implementace tabulky symbolu jako Hash table (prevzata od clena tymu Matuse Motlika) - z predmetu IJC-DU2, projekt 2), 4/2014,

/**
 *  Funkcia podla zadaneho retazca urci index v hash tabulke
 */
unsigned int hash_function(const char *str, unsigned htab_size)
{
  unsigned int h=0;
  const unsigned char *p;

  for(p=(const unsigned char*)str; *p!='\0'; p++)
    h = 65599*h + *p;

  return h % htab_size;
}

/**
 *  Funkcia vytvory a inicializuje hash tabulky s velkostou size
 */
Symbol_table *ST_init(int size) 
{
  Symbol_table *ST_new = malloc( sizeof(Symbol_table) + size * sizeof(Table_item *) );
  if (ST_new == NULL) {
    fprintf(stderr, "Malloc error!\n");
    return NULL;
  }

  ST_new->size = size;
  for (int i = 0; i < size; i++) {
    ST_new->ptr[i] = NULL;
  }

  return ST_new;
}

/**
 *  Funkcia vyhlada zaznam v hash tabulke ktory odpoveda retazcu key a vrati ho
 *    v pripade ze ho nenajde vrati NULL
 */
Table_item* ST_find(Symbol_table *t, string *key)
{
  // vypocita index v hash tabulke podla zadaneho key
  unsigned int i = hash_function(key->str, t->size);

  Table_item *item = t->ptr[i];

  while (item != NULL) {
    // Naslo key v hash tabulke
    if (strcmp(item->key->str, key->str) == 0) {
      return item;
    }
    item = item->next;
  }

  return NULL;
}

/**
 *  Funkcia vyhlada zaznam v hash tabulke ktory odpoveda retazcu key
 *  ak ho naslo vrati chybu, inac vytvori novy prvok a ulozi hodnotu typu premennej
 */
int ST_save(Symbol_table *t, string *key, eType_of_item item_type, int type)
{
  // vypocita index v hash tabulke podla zadaneho key
  unsigned int i = hash_function(key->str, t->size);

  Table_item *item = t->ptr[i];
  Table_item *previous_item = NULL;

  while (item != NULL) {
    // Naslo key v hash tabulke
    if (strcmp(item->key->str, key->str) == 0) {
      return SEM_ERROR_FUNCT_VAR;
    }
    previous_item = item;
    item = item->next;
  }

  // vytvorenie novej polozky
  Table_item *newitem = (Table_item *) malloc(sizeof(Table_item));
   if (newitem == NULL) {
    fprintf(stderr, "Malloc error\n");
    return RUNTIME_ERROR_OTHERS;
  }

  // alokovanie pamati pre uData
  newitem->data = malloc(sizeof(uData));
  if (newitem->data == NULL) {
    free(newitem);
    fprintf(stderr, "Malloc error\n");
    return RUNTIME_ERROR_OTHERS;
  }

  
  if (previous_item == NULL) 
    t->ptr[i] = newitem;
  else
    previous_item->next = newitem;

  // ulozenie key do hash tabulky
  newitem->key = key;

  // ulozenie typu polozky(premenna/funkcia)
  newitem->type = item_type;
  // polozka pre premennu
  if (item_type == type_variable) {
    newitem->data->var.var_type = type;
  
    if ((newitem->data->var.var_value = malloc (sizeof(uValue))) == NULL) {
      fprintf(stderr, "Malloc error\n");
      free(newitem->data);
      free(newitem->key->str);
      free(newitem->key);
      free(newitem);
      return RUNTIME_ERROR_OTHERS;
    }
  }
  // polozka pre funkciu
  else {
    if ((newitem->data->func.local_ST = ST_init(TABLE_SIZE)) == NULL)
      return RUNTIME_ERROR_OTHERS;

    if ((newitem->data->func.params = malloc (sizeof(string))) == NULL) {
      ST_free(newitem->data->func.local_ST);
      return RUNTIME_ERROR_OTHERS;
    }

    // alokovanie miesta pre parametry
    if (strInit(newitem->data->func.params) != STR_SUCCESS) {
      ST_free(newitem->data->func.local_ST);
      free(newitem->data->func.params);
      return RUNTIME_ERROR_OTHERS;
    }

    // alokovanie miesta pre parametre funkcie
    if ((newitem->data->func.param_names = malloc(sizeof(Par_names))) == NULL) {
      ST_free(newitem->data->func.local_ST);
      free(newitem->data->func.params);
      return RUNTIME_ERROR_OTHERS;
    }
    newitem->data->func.param_names->param = NULL;
    
  }

  newitem->next = NULL;
  return OK;
}

/**
 *  Funkcia vymaze vsetky polozky hash tabulky
 */
void ST_clear(Symbol_table *t) 
{
  Table_item *item;
  Table_item *last;
  Par_names *param_item;
  Par_names *pom_item;

  for (int i = 0; i < t->size; i++) {
    item = t->ptr[i];
    t->ptr[i] = NULL;
    while (item != NULL) {
      last = item;
      item = item->next;
      if (last->type == type_function) {
        ST_clear(last->data->func.local_ST);
        free(last->data->func.local_ST);
        //free(last->data->func.instr);
        free(last->data->func.params->str);
        free(last->data->func.params);
        param_item = last->data->func.param_names;
        if (param_item->param == NULL) {
          free(param_item);
        }
        else {
          while (param_item != NULL) {
            pom_item = param_item;
            param_item = param_item->next_param;
            free(pom_item);
          }
        }

      }
      else {
        if (last->data->var.var_type == STRING)
          free(last->data->var.var_value->typ_string.str);
        free(last->data->var.var_value);
      }
      free(last->key->str);
      free(last->key);
      free(last->data);
      free(last);
    }
  }
}

/**
 *  Funkcia zrusi hash tabulku 
 */
void ST_free(Symbol_table *t)
{
  ST_clear(t);
  free(t);
  t = NULL;
}

/**
 *  Funkcia zavola zadanu funkciu pre kazdy prvok
 */
void ST_foreach(Symbol_table *t, void (*function)(const char*, int, int)) 
{
  if (t == NULL) {
    fprintf(stderr, "Wrong pointer to hast table!\n");
    return;
  }

  Table_item *item;
  int type = -1;

  for (int i = 0; i < t->size; i++) {
    for (item = t->ptr[i]; item != NULL; item = item->next) {
      type = (item->type == type_function ? item->data->func.func_type : item->data->var.var_type);
      function(item->key->str, item->type, type);
      if (item->type == 1) {
        printf("\tforvard: %d", item->data->func.forvard);
      }
      printf("\n");
    }
  }
}

/**
 *  Funkcia pre vypis
 */
void print(const char* key, int item_type, int type)
{
  printf("%s\t\t%d\t\t%d", key, item_type, type);
}

/**
 *  Funkcia vypise priemernu, min. a max. dlzky zoznamu v hash tabulke
 */
void ST_statistics(Symbol_table *t)
{
  int max = -1;
  int min = 999999;
  double average = 0.0;
  Table_item *item;
  int j = 0;

  for (int i = 0; i < t->size; i++) {
    for (item = t->ptr[i], j = 0; item != NULL; item = item->next, j++) {
      average++;
    }
    if (j > max)
      max = j;
    if (j < min)
      min = j;
  }

  average = average / t->size;

  printf("\nMax: %d\nMin: %d\nAverage: %f\n", max, min, average);
}
