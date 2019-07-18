/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: sa_pa.c
* Autor: Marcin Juraj
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

/* include hlavickovych suborov */
#include "sa_pa.h"
#include "scaner.h"
#include "ilist.h"
#include "ial.h"
#include <string.h>

#define STR_LEN_INC 8
/*Globalne premenne */

 int akt_radek;
 t_token aktualniToken = {-1,NULL} ;
 t_token PomAktualniToken = {-1,NULL} ;
 t_token* result;
 tInst ins;
 Symbol_table *Global_ST,*table; 
 tInstructionList *list;
 Table_item *Posl_Inst ;
 list_item* zasobnik= NULL;
 Table_item *item, *pom_item, *medzivysledok;
 

int counterVar = 1;

int Error = OK;
int SA_hash_table[TO_COUNT+3];

/* Tabulka PA - pravidiel podla ktorych sa bude vykonavat PA */
int SA_PA_table[dolar_pa+1][dolar_pa+1] =
{
//     (           )         + -        * /          cmp         i             $ (; end)           stack
	{lesser_pa,	equal_pa,	lesser_pa,	lesser_pa,	lesser_pa,	lesser_pa,	  error_pa},    // for (
	{error_pa,	greater_pa,	greater_pa,	greater_pa,	greater_pa,	error_pa,   greater_pa},	// for )
	{lesser_pa,	greater_pa,	greater_pa,	lesser_pa,	greater_pa,	lesser_pa,	greater_pa},	// for + -
	{lesser_pa,	greater_pa,	greater_pa,	greater_pa,	greater_pa,	lesser_pa,	greater_pa},	// for * /
	{lesser_pa,	greater_pa,	lesser_pa,	lesser_pa,	greater_pa,	lesser_pa,	greater_pa},	// for cmp
	{error_pa,	greater_pa,	greater_pa,	greater_pa,	greater_pa,	error_pa,	greater_pa},	// for i
	{lesser_pa,	error_pa,	lesser_pa,	lesser_pa,	lesser_pa,	lesser_pa,	  error_pa}		// for $
};

/*jednotlive pravidla, ktore sa budu uplatnovat pri redukcii*/
const t_token rule_1[3] = {{ID,NULL},					{-1,NULL},					{-1,NULL}};   			 // pravidlo E -> i
const t_token rule_2[3] = {{RIGHT_BRACKET,NULL},		{expression_se,NULL},		{LEFT_BRACKET,NULL}};    // pravidlo E -> (E)
const t_token rule_3[3] = {{expression_se,NULL},		{operator_se,NULL},			{expression_se,NULL}};   // pravidlo E -> E opr E

/*Pole v ktorom su tokeny prevedene na symboly, ktore sa pomocou ktorych
  sa potom budeme riadit v tabulke PA */
void sa_hash_table_init()
{
	for (int i=0; i<TO_COUNT; i++)
	{
		SA_hash_table[i] = dolar_pa;
	}

	SA_hash_table[LEFT_BRACKET] = left_bracket_pa;

	SA_hash_table[RIGHT_BRACKET] = right_bracket_pa;

	SA_hash_table[PLUS] = plus_minus_pa;
	SA_hash_table[MINUS] = plus_minus_pa;

	SA_hash_table[KRAT] = multiply_divide_pa;
	SA_hash_table[DELENIE] = multiply_divide_pa;

	SA_hash_table[SMALL] = compare_operator_pa;
	SA_hash_table[BIG] = compare_operator_pa;
	SA_hash_table[SMALL_OR_EQUAL] = compare_operator_pa;
	SA_hash_table[BIG_OR_EQUAL] = compare_operator_pa;
	SA_hash_table[EQUAL] = compare_operator_pa;
	SA_hash_table[NOT_EQUAL] = compare_operator_pa;

	SA_hash_table[ID] = identificator_pa;
	SA_hash_table[INTEGER_NUMB] = identificator_pa;
	SA_hash_table[DOUBLE_NUMB] = identificator_pa;
	SA_hash_table[C_STRING] = identificator_pa;
	SA_hash_table[TRUE_KW] = identificator_pa;
	SA_hash_table[FALSE_KW] = identificator_pa;
    
	SA_hash_table[SEMICOLON] = dolar_pa;
	SA_hash_table[END] = dolar_pa;
	SA_hash_table[DO] = dolar_pa;
	SA_hash_table[THEN] = dolar_pa;
	SA_hash_table[END_OF_PROGRAM] = dolar_pa;
}

/* Funkcia ktora nam nainicializuje zasobnik */
void Stack_init(list_item** vrchol)
{
	if ((*vrchol = (list_item*) malloc(sizeof(list_item))) != NULL)
	{
		(*vrchol)->hodnota = END_OF_FILE;
		(*vrchol)->value = NULL;
		(*vrchol)->left = NULL;
		(*vrchol)->right = NULL;
	}
	else
	{
		Error = RUNTIME_ERROR_VAR;
	}
}

 #ifdef DEBUG
/* Pomocna funkcia Stack_work_out ktora sluzi na vypis toho co sa nachaza v danej chvili
   na zasobniku vysledok ulozi do suboru. NEOVPLIVNUJE BEH PROGRAMU! */
void Stack_work_out(list_item* vrchol, char * text)
{
	FILE* soubor = fopen("./sa_expr.txt","w");
	fprintf(soubor, "   %s: \n", text);
	fprintf(soubor, "   ");
	while (vrchol != NULL)
	{
		switch (vrchol->hodnota)
		{
			case ID:
				fprintf(soubor,"i; ");
				break;
			case INTEGER_NUMB:
				fprintf(soubor,"i; ");
				break;
			case DOUBLE_NUMB:
				fprintf(soubor,"i; ");
				break;
			case LEFT_BRACKET:
				fprintf(soubor,"(; ");
				break;
			case RIGHT_BRACKET:
				fprintf(soubor,"); ");
				break;
			case COMMA:
				fprintf(soubor,",; ");
				break;
			case PLUS:
				fprintf(soubor,"+; ");
				break;
			case MINUS:
				fprintf(soubor,"-; ");
				break;
			case KRAT:
				fprintf(soubor,"*; ");
				break;
			case DELENIE:
				fprintf(soubor,"/; ");
				break;
			case SMALL:
				fprintf(soubor,"<_; ");
				break;
			case SMALL_OR_EQUAL:
				fprintf(soubor,"<=; ");
				break;
			case BIG_OR_EQUAL:
				fprintf(soubor,">=; ");
				break;
			case EQUAL:
				fprintf(soubor,"=; ");
				break;
			case NOT_EQUAL:
				fprintf(soubor,"<>; ");
				break;
			case END_OF_FILE:
				fprintf(soubor,"$; ");
				break;
			case expression_se:
				fprintf(soubor,"E; ");
				break;
			default:
				fprintf(soubor,"%d ", vrchol->hodnota);
				break;
		}
		vrchol = vrchol->right;
	}
	fprintf(soubor, "\n\n");
	fclose(soubor);
}
#endif

/* Funkcia ktora najde na zasobniku znak zarazky, ktory urcuje po miesto
   pokial treba zo zasobnika jednotlive symboly zredukovat */
list_item* find_lesser(list_item* vrchol)
{
	while (vrchol != NULL)
	{
		if (vrchol->hodnota == lesser_se)
		{
			break;
		}
		vrchol = vrchol->right;
	}
	return vrchol;
}

/* Funkcia ktora najde na zasobniku terminal a vrati ho */
list_item* find_terminal(list_item* vrchol)
{
	while (vrchol != NULL)
	{
		if (vrchol->hodnota <= END_OF_FILE)
		{
			break;
		}
		vrchol = vrchol->right;
	}
	return vrchol;
}

/* Funkcia, ktora ulozi dany neterminal na zasobnik a posunie vrchol zasobnika tak
   aby ukazoval na dany neterminal */
void Push(list_item** vrchol, list_item* pred, list_item *polozka, t_token *tokenik)
{
	list_item* pom;
	if ((pom = (list_item*) malloc(sizeof(list_item))) != NULL)
	{
		pom->left = NULL;
		pom->right = pred;
        pom->hodnota = tokenik->stav;
		pom->value = polozka->value; 

		if (pred != NULL)
		{
			pom->left = pred->left;
			if (pred->left != NULL)
			{
				pred->left->right = pom;
			}
			pred->left = pom;
		}
		if (pom->left == NULL)
		{
			*vrchol = pom;
		}
	}
	else
	{
		Error = RUNTIME_ERROR_VAR;
	}
}

/* Funkcia, ktora zabezpeci vyprazdnenie zasobnika po najblizsiu zarazku,
   aby sa potom mohlo podla niektoreho z pravidiel redukovat */
int Pop_rule(list_item** vrchol, list_item* pole)
{
	int count = 0;
	list_item* pom;
	pole[0].hodnota = -1;
	pole[0].value = NULL;
	pole[1].hodnota = -1;
	pole[1].value = NULL;
	pole[2].hodnota = -1;
    pole[2].value = NULL;

	if (*vrchol != NULL)
	{
		while (*vrchol != NULL && (*vrchol)->hodnota != END_OF_FILE && (*vrchol)->hodnota != lesser_se)
		{
			//printf("ATRIBUT E-cka %s \n",(*vrchol)->value.atribut->t_STRING->str);
			if (count < 3)
			{
				pole[count].hodnota = (*vrchol)->hodnota;
				pole[count].value = (*vrchol)->value;
			}
			 #ifdef DEBUG
		      printf("v pop rule vrchol->hodnota: %d \n",(*vrchol)->hodnota);
		     #endif
			count++;
			pom = *vrchol;
			*vrchol = (*vrchol)->right;
			free(pom);
		}
		if ((*vrchol)->hodnota == lesser_se)
		{
			pom = *vrchol;
			*vrchol = (*vrchol)->right;
			free(pom);
			if (*vrchol != NULL)
			{
				(*vrchol)->left = NULL;
			}
			return count < 4;
		}
	}
	return 0;
}

/* Funkcia, ktora uvolni zasobnik */
void Dispose(list_item** vrchol)
{
	list_item* pom;
	while (*vrchol != NULL)
	{
		pom = *vrchol;
		*vrchol = (*vrchol)->right;
		free(pom);
	}
}

/* Funkcia na porovnanie jednotlivych pravidiel.
   Podla vysledku porovnania sa potom vyberie prave jedno
   alebo ziadne pravidlo, ktore sa ma pouzit na redukciu  */
int rules_equals(t_token* pole1, const t_token* pole2)
{
	for (int i = 0; i < 2; i++)
	{
		if (pole1[i].stav != pole2[i].stav)
		{
			return 0;
		}
	}
	return 1;
}

/* Hlavna funkcia ktora sluzi k syntaktickej PA analyze. Funkcia ma 4 parametre ukazatel na Globalnu a Lokalnu
   tabulku symbolov, ukazatel na list instrukcii, do ktoreho sa budu pridavat postupne jednotlive instrukcie,
   a ukazatel na 1. token ktory sa ma spacovat  */
int precedencna_SA(Symbol_table *Gtable,Symbol_table *Ltable,tInstructionList *list_instr, t_token *token,Table_item **E)
{
	int get_next_rule, a, b,koniec;
	int koniec_pom_item = 1;


	Global_ST = Gtable;
	table = Ltable;
	list = list_instr;
	
	list_item* pom = NULL;
	list_item tpom;
	list_item rule[3];
   
    tpom.value = NULL;
	
	/* Pomocne premenne get_next_rule - slusi na rozhodovanie o tom 
	   ci mam v nasledujucom kroku citat token zo vstupu alebo redukovat */
	
     rule[0].hodnota = -1;
     rule[0].value = NULL;
		rule[1].hodnota = -1;
		rule[1].value = NULL;
	rule[2].hodnota = -1;
    rule[2].value = NULL;

	t_token rule_abstract[3];
   
   /* Pomocny subor, sluzi na debugovanie programu, uklada sa do neho postupne 
      to co sa nachadza na zasobniku pomocou funkcie Stack_work_out */
	sa_hash_table_init();

	// vlatni algoritmus
    //   printf("%s\n",token->atribut->t_STRING->str );
	Stack_init(&zasobnik);
	if (Error)
	{
		return Error;
	}
	
   /* Hned ako nacitam token potrebujem zisti:
		   1. Ak je to ID tak ci bolo deklarovane, ak ano tak ho hned ulozim do premennej *item
		   2. alebo ci to je priama hodnota a potom je nutne vygenerovat pomocnu premennu 
		   a tu ulozit do premennej *item */
     if (token->stav == ID)
    {
       FIND_IN_TABLE(item);	
    }
    else if (token->stav == INTEGER_NUMB)
    {
      CREATE_ITEM_OTHER(INTEGER);
      item->data->var.var_value->typ_integer = token->atribut->t_INTEGER;         
    }
    else if (token->stav == DOUBLE_NUMB)
    {
         CREATE_ITEM_OTHER(REAL);
         item->data->var.var_value->typ_real = token->atribut->t_REAL;
    }
     else if (token->stav == C_STRING)
    {
         CREATE_ITEM(STRING);
         item->data->var.var_value->typ_string.length = token->atribut->t_STRING->length;
         item->data->var.var_value->typ_string.allocSize = token->atribut->t_STRING->allocSize;
    }
    else if (token->stav == TRUE_KW || token -> stav == FALSE_KW )
    {
    	CREATE_ITEM_OTHER(BOOLEAN);
    	item->data->var.var_value->typ_boolean = (token->stav == TRUE_KW ? true : false);
    }
  
	/* testovanie jednotlivych pravidiel */
	do
	{
		get_next_rule = 1;

		a = SA_hash_table[token->stav];		
		pom = find_terminal(zasobnik);

		b = SA_hash_table[pom->hodnota];
	
       // x=SA_PA_table[b][a];
        // Stack_work_out(zasobnik, "pred");
        #ifdef DEBUG
		printf("pred switchom %d %d %d \n",b,a ,SA_PA_table[b][a]);
		#endif

		/* Podla indexov v tabulke SA_PA_table sa vyberie jedno z pravidiel: 
		   equal_pa, lesser_pa, greater_pa alebo error_pa a podla tohto pravidla 
		   sa potom vykonavaju prislusne akcie  */
		switch (SA_PA_table[b][a])
		{
			/* pravidlo equal_pa vykona to ze iba prepise vstupny token na zasobnik */
			case equal_pa:

			  tpom.value = NULL;
				Push(&zasobnik, zasobnik, &tpom,token);
				if (Error)
				{
					DISPOSE_ALL();
					return Error;
				}
			//	Stack_work_out(zasobnik, "equals");
				break;

           /* Pravidlo lesser_pa pushne vstupny token na zasobnik a to tak, ze
              najprv vlozi zarazku a potom az samotny token */
			case lesser_pa:
			
			/* siftovanie zarazky na zasobnik */
				aktualniToken.stav = lesser_se;
				tpom.value = NULL;
				Push(&zasobnik, find_terminal(zasobnik), &tpom,&aktualniToken);
				if (Error)
				{					
					DISPOSE_ALL();
					return Error;
				}

				/* musim osetrit ci je ten vstupny token INTEGER_NUMB, DOUBLE_NUMB 
				   alebo C_STRING, ak ano tak ich musim brat ako ID (koli redukcnemu pravidlu) */
				tpom.value = item;
				// printf("%s\n",tpom.value->data->var.var_value->typ_string.str );
				if (token->stav == INTEGER_NUMB)
				{
					aktualniToken.stav = ID;
					Push(&zasobnik, zasobnik, &tpom,&aktualniToken);
					if (Error)
				     {
					   DISPOSE_ALL();
					   return Error;
				     }
				}
				else if (token->stav == DOUBLE_NUMB)
				{
					aktualniToken.stav = ID;
					Push(&zasobnik, zasobnik, &tpom,&aktualniToken);
					if (Error)
			       	{
				    	DISPOSE_ALL();
				    	return Error;
			     	}
				}
				else if (token->stav == C_STRING)
				{
					aktualniToken.stav = ID;
					Push(&zasobnik, zasobnik, &tpom,&aktualniToken);
					if (Error)
			       	{
				    	DISPOSE_ALL();
				    	return Error;
			     	}
			     //	printf("%s\n",tpom.value->data->var.var_value->typ_string.str );
				}
				else if (token->stav == TRUE_KW || token -> stav == FALSE_KW )
				{
					aktualniToken.stav = ID;
					Push(&zasobnik, zasobnik, &tpom,&aktualniToken);
					if (Error)
			       	{
				    	DISPOSE_ALL();
				    	return Error;
			     	}

				}
				else 
				 {
				   Push(&zasobnik, zasobnik, &tpom,token);
				   if (Error)
				   {
				   	DISPOSE_ALL();
					return Error;
				   }
			     }
			  #ifdef DEBUG
				Stack_work_out(zasobnik, "lesser");
		//		printf("koniec lasser %d \n",zasobnik->value->data->var.var_type);
			  #endif
				break;

            /* Najvecsie pravidlo greater_pa sluzi na redukciu takze najpr vyprazdni zasobnik
               pomocou funkcie Pop_rule, ktore si veci ktore vyprazdnuje uklada do premennej rule
               co je pole list_item a potom naslednym porovnavanim rozhodne, ktore redukcne pravidlo
               sa  ma pouzit */
			case greater_pa:
				get_next_rule = 0;
		    	//	Stack_work_out(zasobnik, "pred");
			    //	printf("ATRIBUT E-cka %s \n",zasobnik->value.atribut->t_STRING->str);
				if (Pop_rule(&zasobnik, rule))
				{
					// printf("v pop_rule in greater_PA  \n");
				    // Stack_work_out(zasobnik, "greater");
					rule_abstract[0].stav = rule[0].hodnota;
					rule_abstract[1].stav = rule[1].hodnota;
					rule_abstract[2].stav = rule[2].hodnota;
				 if (rule[0].hodnota != -1 && (SA_hash_table[rule[1].hodnota] == plus_minus_pa ||
											SA_hash_table[rule[1].hodnota] == multiply_divide_pa ||
											SA_hash_table[rule[1].hodnota] == compare_operator_pa ))
					{
						rule_abstract[1].stav = operator_se;
					}
                 
/*************************************************************************************/
/* Pravidlo, ktore vykona redukciu podla 1. Pravidla : E -> i 
   porovnam ci ra to co mam na zasobniku zhoduje z danym pravidlom 
   ak ano tak ho zacnem vykonavat */					
/************************************************************************************/
					if (rules_equals(rule_abstract, rule_1))
					{
    					 aktualniToken.stav = expression_se;
						 tpom.value = item;

						Push(&zasobnik, zasobnik, &tpom,&aktualniToken);
						if (Error)
						{
							DISPOSE_ALL();
							return Error;
						}
                      #ifdef DEBUG  
    					printf("ATRIBUT E %s \n",item->key->str);		 
						Stack_work_out(zasobnik, "pravidlo 1");
                      #endif
    				// printf("%s\n",tpom.value->data->var.var_value->typ_string.str );

					/* Ak som uz vyprazdnil zasobnik a nemam nic na vstupe tak koncim */ 	
						if (STACK_IN_END_STATE && a == dolar_pa) 
						{ 
							result = token;  
						   
								*E = tpom.value;
                             #ifdef DEBUG
    				         //  printf("%s\n",item->data->var.var_value->typ_string.str );
    					     //  printf("ATRIBUT E-cka pred getnexToken %d \n",E->data->var.var_type);	
						     #endif
							DISPOSE_ALL(); 
							return OK; 
						} 

					}
/************************************************************************************/
/* Pravidlo ktore vykona redukciu podla 2. Pravidla: E -> (E) 
   porovnam ci sa to co mam na zasobniku zhoduje z danym pravidlom 
   ak ano tak ho zacnem vykonavat */
/************************************************************************************/
				else if (rules_equals(rule_abstract, rule_2))
					{
					    aktualniToken.stav= expression_se;
					    if (koniec_pom_item == 0 )
					      {
						    tpom.value = pom_item;
					      }
					    else
                          {
                          	tpom.value = item;
                          }
						Push(&zasobnik, zasobnik, &tpom, &aktualniToken);
						if (Error)
						{
							DISPOSE_ALL();
							return Error;
						}
						 #ifdef DEBUG	
					        Stack_work_out(zasobnik, "pravidlo 2");
						    printf("pravidlo 2 \n");

                          printf ("\n \nkoniec_pom_item %d \n \n", koniec_pom_item);
                           printf ("\n \n()%s \n \n", pom_item->key->str);
                          #endif
					/* Ak som uz vyprazdnil zasobnik a nemam nic na vstupe tak koncim */ 	
						if (STACK_IN_END_STATE && a == dolar_pa) 
						{ 
							result = token; 

							  *E = tpom.value;
					         
					          #ifdef DEBUG	
					            printf ("\n \nkoniec_pom_item ()%d \n \n", koniec_pom_item);
                                printf ("\n \nspravne%s \n \n", pom_item->key->str);
					            printf ("\n \nmoje%s \n \n", (*E)->key->str);
                              #endif

							DISPOSE_ALL(); 
							return OK; 
						} 
						 if (token->stav != (END || END_OF_PROGRAM || DO || THEN || SEMICOLON) && token->stav !=RIGHT_BRACKET)
						{
							koniec_pom_item = 1;
							#ifdef DEBUG	
					            printf ("koniec_pom_item ()%d \n \n", koniec_pom_item);
                            #endif
					     }
					}					
/*********************************************************************************/
  /* Pravidlo ktore vykona redukciu podla 3. Pravidla:  E -> E oper E 
   porovnam ci ra to co mam na zasobniku zhoduje z danym pravidlom 
   ak ano tak ho zacnem vykonavat */
/*********************************************************************************/
				else if (rules_equals(rule_abstract, rule_3))
					{
                        #ifdef DEBUG	
						   printf("pravidlo 3 \n");
						   printf("v pop_rule in greater_PA %d %d %d\n",rule[0].hodnota,rule[1].hodnota,rule[2].hodnota);
                        #endif

					  if ((rule[0].hodnota == expression_se) && (rule[2].hodnota == expression_se ))
                     {
				       /* Podla toho aku co za znamienko to je tak taku vykonam redukciu */
					   switch (rule[1].hodnota)
                       {
                         case PLUS:
                         	 //===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_1(INTEGER);
									generate_instruction(I_PLUS_INT, rule[2].value, rule[0].value, pom_item);	
								  // printf("ATRIBUT E-cka %d \n",pom_item->data->var.var_value->typ_integer);	
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									CREATE_ITEM_OTHER_1(REAL);
									generate_instruction(I_PLUS_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL  TYP_INTEGER  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                   generate_instruction(I_INT_TO_DOUBLE, rule[2].value, NULL, medzivysledok);
                                   
                                   CREATE_ITEM_OTHER_1(REAL);
                                   generate_instruction(I_PLUS_DOUBLE, rule[0].value, medzivysledok, pom_item) ;
								}

								//===  TYP_INTEGER TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == REAL)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[0].value, NULL, medzivysledok);

                                   CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_PLUS_DOUBLE, rule[2].value, medzivysledok, pom_item) ;
								}

								//===  TYP_STRING TYP_STRING  ================================
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									CREATE_ITEM_2(STRING);
     								generate_instruction(I_SPOJIT_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                                break;

                         case MINUS:
                         				//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_1(INTEGER);;

									generate_instruction(I_MINUS_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									CREATE_ITEM_OTHER_1(REAL);

									generate_instruction(I_MINUS_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL  TYP_INTEGER  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[2].value, NULL, medzivysledok);

                                    CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_MINUS_DOUBLE, medzivysledok,rule[0].value, pom_item) ;
								}

								//===  TYP_INTEGER TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == REAL)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[0].value, NULL, medzivysledok);

                                    CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_MINUS_DOUBLE, rule[2].value, medzivysledok, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case KRAT:
                        				//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_1(INTEGER);
									generate_instruction(I_KRAT_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									 CREATE_ITEM_OTHER_1(REAL);
									generate_instruction(I_KRAT_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								
								//===  TYP_REAL  TYP_INTEGER  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[2].value, NULL, medzivysledok);

                                    CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_KRAT_DOUBLE, medzivysledok,rule[0].value, pom_item) ;
								}

								//===  TYP_INTEGER TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == REAL)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[0].value, NULL, medzivysledok);

                                   CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_KRAT_DOUBLE, rule[2].value, medzivysledok, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case DELENIE:
                          						//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									 CREATE_ITEM_OTHER_1(REAL);
									generate_instruction(I_DELENO_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
								    CREATE_ITEM_OTHER_1(REAL);
									generate_instruction(I_DELENO_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								//===  TYP_REAL  TYP_INTEGER  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[2].value, NULL, medzivysledok);

                                    CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_DELENO_DOUBLE, medzivysledok,rule[0].value, pom_item) ;
								}

								//===  TYP_INTEGER TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == REAL)
								{
									CREATE_ITEM_OTHER_2(REAL);
                                    generate_instruction(I_INT_TO_DOUBLE, rule[0].value, NULL, medzivysledok);

                                   CREATE_ITEM_OTHER_1(REAL);
     								generate_instruction(I_DELENO_DOUBLE, rule[2].value, medzivysledok, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case SMALL:
                          						//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									  CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_MENSI_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_MENSI_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_MENSI_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								
								else if (rule[0].value->data->var.var_type == BOOLEAN && rule[2].value->data->var.var_type == BOOLEAN)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								 generate_instruction(I_MENSI_BOOLEAN, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case BIG:
                          		//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_VETSI_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_VETSI_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_VETSI_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								
								else if (rule[0].value->data->var.var_type == BOOLEAN && rule[2].value->data->var.var_type == BOOLEAN)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_VETSI_BOOLEAN, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case BIG_OR_EQUAL:

								//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_VETSIROVNO_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_VETSIROVNO_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_VETSIROVNO_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								
								else if (rule[0].value->data->var.var_type == BOOLEAN && rule[2].value->data->var.var_type == BOOLEAN)
								{
									CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_VETSIROVNO_BOOLEAN, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;
                           
                          case SMALL_OR_EQUAL:
                          					//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_MENSIROVNO_INT, rule[2].value, rule[0].value, pom_item);

								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_MENSIROVNO_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_MENSIROVNO_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								else if (rule[0].value->data->var.var_type == BOOLEAN && rule[2].value->data->var.var_type == BOOLEAN)
								{
									CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_MENSIROVNO_BOOLEAN, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case EQUAL:
                          					//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_ROVNO_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_ROVNO_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_ROVNO_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								else if (rule[0].value->data->var.var_type == BOOLEAN && rule[2].value->data->var.var_type == BOOLEAN)
								{
									CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_ROVNO_BOOLEAN, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                          case NOT_EQUAL:
                          						//===  TYP_INTEGER TYP_INTEGER  ================================
								if (rule[0].value->data->var.var_type == INTEGER && rule[2].value->data->var.var_type == INTEGER)
								{
									CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_NEROVNO_INT, rule[2].value, rule[0].value, pom_item);
								}

								//===  TYP_REAL TYP_REAL  ================================
								else if (rule[0].value->data->var.var_type == REAL && rule[2].value->data->var.var_type == REAL)
								{								
									 CREATE_ITEM_OTHER_1(BOOLEAN);
									generate_instruction(I_NEROVNO_DOUBLE, rule[2].value, rule[0].value, pom_item);
								}
								else if (rule[0].value->data->var.var_type == STRING && rule[2].value->data->var.var_type == STRING)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_NEROVNO_STRING, rule[2].value, rule[0].value, pom_item) ;
								}
								else if (rule[0].value->data->var.var_type == BOOLEAN && rule[2].value->data->var.var_type == BOOLEAN)
								{
									 CREATE_ITEM_OTHER_1(BOOLEAN);
     								generate_instruction(I_NEROVNO_BOOLEAN, rule[2].value, rule[0].value, pom_item) ;
								}
								else
								{
								  return SEM_ERROR_ARITH_EXPR;
								}
                          break;

                         default:
								DISPOSE_ALL();
								return SYNTAX_ERROR;

                       }
                       aktualniToken.stav= expression_se;
                       tpom.value = pom_item;

                       #ifdef DEBUG	
                          printf ("\n \ntpom.value->data->var.var_type %d \n \n", tpom.value->data->var.var_type);
                       #endif

                       Push(&zasobnik, zasobnik, &tpom, &aktualniToken);
						if (Error)
						{
							DISPOSE_ALL();
							return Error;
						}

                         koniec = token->stav;
                         #ifdef DEBUG	
                          printf ("\n \nkoniec- token stav  %d \n \n", koniec);
					     #endif	
						if (koniec == RIGHT_BRACKET)
						{
							koniec_pom_item = 0;
						}

					   #ifdef DEBUG	
                          printf ("\n \n %d \n \n", koniec_pom_item);
					      printf ("\n \n operator %s \n \n", tpom.value->key->str);
                       #endif	
					}
					else
					{
						DISPOSE_ALL();
						return SYNTAX_ERROR;
					}

                      //   Stack_work_out(zasobnik, "pravidlo 3");
						if (STACK_IN_END_STATE && a == dolar_pa) 
						{ 
							result = token;  							
    					    *E = tpom.value;

							DISPOSE_ALL(); 
							return OK; 
						} 

				    }
				else
				{    
					pom =find_terminal(zasobnik);

					#ifdef DEBUG	
					Stack_work_out(zasobnik, "neni pravidlo ");
					#endif

					DISPOSE_ALL();
					return SYNTAX_ERROR;					
				}
				break;
            }
       /* Ak sa v tabulke nachadza prazdne miesto - ERROR tak idem do tohto stavu */     
		case error_pa:
		  KONECNY_STAV();
		  return SYNTAX_ERROR;
		break;

		default:
		 DISPOSE_ALL();
		 return INTERPRET_ERROR;
		}

       /* Zistim ci mam nacitavat dalsi token  */
		if (get_next_rule)
		{  
			
		    getNextToken(token);
		    #ifdef DEBUG	
		    	printf("sme po getNextToken %d \n",token->stav);
		    #endif	
			if (token->stav == LEX_ERROR || token->stav == INTERPRET_ERROR)
			{
				DISPOSE_ALL();
				return token->stav;
			}
		/* Hned ako nacitam token potrebujem zisti:
		   1. Ak je to ID tak ci bolo deklarovane
		   2. alebo ci to je priama hodnota a potom je nutne vygenerovat pomocnu premennu */
		 if (token->stav == ID)
          {
            FIND_IN_TABLE(item);
           }
          else if (token->stav == INTEGER_NUMB)
 		   {
 			   
    	      CREATE_ITEM_OTHER(INTEGER);
    		  item->data->var.var_value->typ_integer = token->atribut->t_INTEGER;   
   		   }
  		   else if (token->stav == DOUBLE_NUMB)
    		{   		
              CREATE_ITEM_OTHER(REAL);
              item->data->var.var_value->typ_real = token->atribut->t_REAL;
            }
             else if (token->stav == C_STRING)
   		    {
      		   CREATE_ITEM(STRING);
    	      item->data->var.var_value->typ_string.length = token->atribut->t_STRING->length;
       		  item->data->var.var_value->typ_string.allocSize = token->atribut->t_STRING->allocSize;
            } 
            else if (token->stav == TRUE_KW || token -> stav == FALSE_KW )
   			 {
    			CREATE_ITEM_OTHER(BOOLEAN);
    			item->data->var.var_value->typ_boolean = (token->stav == TRUE_KW ? true : false);
   			 }
   			
  
	    }
	  

 } while (1	);

	DISPOSE_ALL();
	//printf("Fakt by ma zajimalo, jak jsem do tohodle bodu dosel!\n");
	return INTERPRET_ERROR;
// konec vlastniho algoritmu
}

/* Funkcie na generovanie jednotlivych instrukcii pre aritmeticke vyrazy */

void generateVariable(string *var)
// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace

{
  strClear(var);
  strAddChar(var, '$');
  int i;
  i = counterVar;
  while (i != 0)
  {
    strAddChar(var, (char)(i % 10 + '0'));
    i = i / 10;
  }
  counterVar ++;
} 
