
/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvoøil: Petr Pøikryl, listopad 1994
**                                         Úpravy: Andrea Nìmcová, prosinec 1995
**                                                      Petr Pøikryl, duben 1996
**                                                   Petr Pøikryl, listopad 1997
**                                  Pøevod do jazyka C: Martin Tuèek, øíjen 2005
**                                         Úpravy: Bohuslav Køena, listopad 2009
**                                         Úpravy: Karel Masaøík, øíjen 2013
**                                         Úpravy: Radek Hranický, øíjen 2014
**
** Implementujte rekurzivním zpùsobem operace nad binárním vyhledávacím
** stromem (BVS; v angliètinì BST - Binary Search Tree).
**
** Klíèem uzlu stromu je jeden znak (obecnì jím mù¾e být cokoliv, podle
** èeho se vyhledává). U¾iteèným (vyhledávaným) obsahem je zde integer.
** Uzly s men¹ím klíèem le¾í vlevo, uzly s vìt¹ím klíèem le¾í ve stromu
** vpravo. Vyu¾ijte dynamického pøidìlování pamìti.
** Rekurzivním zpùsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíèem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zru¹ení uzlu se zadaným klíèem
**   BSTDispose ... zru¹ení celého stromu
**
** ADT BVS je reprezentován koøenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíè (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). Pøesnou definici typù 
** naleznete v souboru c401.h.
**
** Pozor! Je tøeba správnì rozli¹ovat, kdy pou¾ít dereferenèní operátor *
** (typicky pøi modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (napø. pøi vyhledávání). V tomto pøíkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, pou¾ijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede poèáteèní inicializaci stromu pøed jeho prvním pou¾itím.
**
** Ovìøit, zda byl ji¾ strom pøedaný pøes RootPtr inicializován, nelze,
** proto¾e pøed první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor vyu¾ívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to pøed vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by toti¾ mohlo vést ke ztrátì pøístupu
** k dynamicky alokované pamìti (tzv. "memory leak").
**	
** V¹imnìte si, ¾e se v hlavièce objevuje typ ukazatel na ukazatel.	
** Proto je tøeba pøi pøiøazení pøes RootPtr pou¾ít dereferenèní operátor *.
** Ten bude pou¾it i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/
	
	*RootPtr = NULL;	
	
	 //solved = FALSE;		  /* V pøípadì øe¹ení sma¾te tento øádek! */	
	
}	

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíèem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v promìnné Content se
** vrací obsah pøíslu¹ného uzlu.´Pokud pøíslu¹ný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah promìnné Content není definován (nic do ní proto
** nepøiøazujte).
**
** Pøi vyhledávání v binárním stromu bychom typicky pou¾ili cyklus ukonèený
** testem dosa¾ení listu nebo nalezení uzlu s klíèem K. V tomto pøípadì ale
** problém øe¹te rekurzivním volání této funkce, pøièem¾ nedeklarujte ¾ádnou
** pomocnou funkci.
**/
							   

	if (RootPtr == NULL)	//nenalezl jsem 
		return FALSE;

	if (RootPtr->Key == K){ // nalezl jsem
		*Content = RootPtr->BSTNodeCont;
		return TRUE;
	}else if (RootPtr->Key > K){ // hledám vlevo
			return BSTSearch(RootPtr->LPtr, K, Content);
	}else if (RootPtr->Key < K){ // hledám vpravo
 		return BSTSearch(RootPtr->RPtr, K, Content);
	}	
	
	
	
	
	 //solved = FALSE;		  /* V pøípadì øe¹ení sma¾te tento øádek! */	
	
} 


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{	
/*   ---------
** Vlo¾í do stromu RootPtr hodnotu Content s klíèem K.
**
** Pokud ji¾ uzel se zadaným klíèem ve stromu existuje, bude obsah uzlu
** s klíèem K nahrazen novou hodnotou. Pokud bude do stromu vlo¾en nový
** uzel, bude vlo¾en v¾dy jako list stromu.
**
** Funkci implementujte rekurzivnì. Nedeklarujte ¾ádnou pomocnou funkci.
**
** Rekurzivní implementace je ménì efektivní, proto¾e se pøi ka¾dém
** rekurzivním zanoøení ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto pøípadì byla efektivnìj¹í jak z hlediska
** rychlosti, tak z hlediska pamì»ových nárokù. Zde jde ale o ¹kolní
** pøíklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/
		
	if (*RootPtr == NULL){
		tBSTNodePtr tmp = malloc(sizeof(struct tBSTNode)); // pøedpoklad uspì¹ného alokování místa (viz forum)
		tmp->Key = K;
		tmp->BSTNodeCont = Content;
		tmp->LPtr = tmp->RPtr = NULL;
		*RootPtr = tmp;
		return;
	}else{
		if ((*RootPtr)->Key == K){
			(*RootPtr)->BSTNodeCont = Content; // nahrazení novou hodnotou
		}else{
			if((*RootPtr)->Key > K){//rekurzivní volání ve správné èásti podstromu
				BSTInsert(&((*RootPtr)->LPtr),K,Content);
			}else{
				BSTInsert(&((*RootPtr)->RPtr),K,Content);
			}
		}
	}
	
	 //solved = FALSE;		  /* V pøípadì øe¹ení sma¾te tento øádek! */	
	
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, pøesun a uvolnìní nejpravìj¹ího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude pøesunuta hodnota
** nejpravìj¹ího uzlu v podstromu, který je urèen ukazatelem RootPtr.
** Pøedpokládá se, ¾e hodnota ukazatele RootPtr nebude NULL (zajistìte to
** testováním pøed volání této funkce). Tuto funkci implementujte rekurzivnì. 
**
** Tato pomocná funkce bude pou¾ita dále. Ne¾ ji zaènete implementovat,
** pøeètìte si komentáø k funkci BSTDelete(). 
**/
    if(*RootPtr == NULL)
        return ;
    else
    {
        tBSTNodePtr tmp = NULL;        
            
        if((*RootPtr)->RPtr == NULL)
        {
            tmp = *RootPtr; 
            PtrReplaced->BSTNodeCont = tmp->BSTNodeCont;
            PtrReplaced->Key = tmp->Key;
            *RootPtr = tmp->LPtr;
            free(tmp);
            return ;
        }
        else if((*RootPtr)->RPtr != NULL)
        {
            ReplaceByRightmost(PtrReplaced ,&(*RootPtr)->RPtr);
            return ;
        }
    }
    return ;
		
	 //solved = FALSE;		  /* V pøípadì øe¹ení sma¾te tento øádek! */	
	
}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zru¹í uzel stromu, který obsahuje klíè K.
**
** Pokud uzel se zadaným klíèem neexistuje, nedìlá funkce nic. 
** Pokud má ru¹ený uzel jen jeden podstrom, pak jej zdìdí otec ru¹eného uzlu.
** Pokud má ru¹ený uzel oba podstromy, pak je ru¹ený uzel nahrazen nejpravìj¹ím
** uzlem levého podstromu. Pozor! Nejpravìj¹í uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivnì s vyu¾itím døíve deklarované
** pomocné funkce ReplaceByRightmost.
**/


    if(*RootPtr != NULL) //pokud je co zmazat, tak pokracuji
    {
        if((*RootPtr)->Key == K) //nalezl jsem co potrebuji
        {
            tBSTNodePtr tmp = *RootPtr; //vytvorim si pomocnou pormennou
            
            if((*RootPtr)->LPtr == NULL)
            {
                *RootPtr = (*RootPtr)->RPtr;
                free(tmp);
                return ;
            }
            
            else if((*RootPtr)->RPtr == NULL)
            {
                *RootPtr = (*RootPtr)->LPtr;
                free(tmp);
                return ;
            }
            else ReplaceByRightmost(*RootPtr, &((*RootPtr)->LPtr));
        }
        
        else if ((*RootPtr)->Key < K)
        {
            BSTDelete(&((*RootPtr)->RPtr), K);
            return ;
        }
        
        else if ((*RootPtr)->Key > K)
        {
            BSTDelete(&((*RootPtr)->LPtr), K);
            return ;
        }
    }
	
	// solved = FALSE;		  /* V pøípadì øe¹ení sma¾te tento øádek! */	

} 

void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   ----------
** Zru¹í celý binární vyhledávací strom a korektnì uvolní pamì».
**
** Po zru¹ení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivnì bez deklarování pomocné
** funkce.
**/
	if (*RootPtr != NULL){
		if ((*RootPtr)->LPtr != NULL){
			BSTDispose(&(*RootPtr)->LPtr);
		}
		if ((*RootPtr)->RPtr != NULL){
			BSTDispose(&(*RootPtr)->RPtr);
		}
		free(*RootPtr);
	}	
	*RootPtr = NULL;	

	 //solved = FALSE;		  /* V pøípadì øe¹ení sma¾te tento øádek! */	

}

/* konec c401.c */

