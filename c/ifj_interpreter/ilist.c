/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: ilist.c
* Autor: Mour Lukáš
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

 #include "ilist.h"

// Inicializace seznamu
void ListInit (tInstructionList *L)
{
	L->First = NULL;
	L->Last = NULL;
	L->Active = NULL;
}

// Vlozeni prvku na konec seznamu
void InsertLast (tInstructionList *L, tInst NewItem)
{
	tItemList *New;
	New = malloc(sizeof(tItemList));
	if(New == NULL)
	{
		// PORESIT ERROR V PRIPADE MALLOC
		fprintf(stderr, "malloc error");
		ListDispose(L);
	}
	else
	{
		New->Data = NewItem;
		New->NextItem = NULL;
		if(L->First == NULL)
		{
			L->First = New;
		}
		else
		{
			L->Last->NextItem = New;
		}
		L->Last = New;		
	}
}

// Odstraneni (uvolneni) seznamu
void ListDispose (tInstructionList *L)
{
	tItemList *Pom; // Pomocny ukazatel
	while (L->First != NULL)
	{
		Pom = L->First;
		L->First = L->First->NextItem;
		free(Pom);
	}
	ListInit(L); // Stav jako po inicializaci
}

// Nastaveni aktivity na prvni prvek
void FirstActive (tInstructionList *L)
{
	L->Active = L->First;
}

// Posun aktivity na dalsi prvek
void NextActive (tInstructionList *L)
{
	if(L->Active != NULL)
	{
		if(L->Active->NextItem != NULL)
		{
			L->Active = L->Active->NextItem;
		}
		else 
		{
			L->Active = NULL;
		}
	}	
}

// Zkontroluje aktivitu seznamu
int Active(tInstructionList *L)
{
	if(L->Active == NULL)
		return 1;
	else return 0;
}

// Nastaveni aktivity na konkretni prvek
void TargetActive (tInstructionList *L, void *Target)
{
	L->Active = (tItemList*) Target;
}

// Vrati aktualne aktivni instrukci
tInst *CurrentActive (tInstructionList *L)
{
	if(L->Active == NULL)
	{
		// Instrukce neaktivni - PORESIT ERROR
		fprintf(stderr, "zadna instukce neni aktivni");
		return NULL;
	}
	else return &(L->Active->Data);
}

void print_list(tInstructionList *L) {
	tItemList *Pom;
	if (L->First != NULL) {
		Pom = L->First;
		while (Pom != NULL) {
			printf("%d\n", Pom->Data.instType);
			Pom = Pom->NextItem;
		}
	}
}