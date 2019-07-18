/* 
 * interpret IFJ14
 *
*/

#include <stdio.h>
#include <string.h>
#include "str.h"
#include "errors.h"
#include "ial.h"
#include "sa_pa.h"
#include "scaner.h"
#include "interpret.h"
#include "parser.h"
#include <ctype.h>

extern void *main_instr;
Table_item *parametry = NULL;
frame *ptr_stack = NULL;

// pom fce
int is_define(Table_item * t);
void set_define(Table_item *t);
void nastav_loc(Table_item **t, frame * p);
int fLENGTH(char *s);
char * fCOPY(char *s, int i, int n);
void uvolni_seznam (Table_item * t);
// pom fce debug
void pom(int i, Table_item *t);
void vypis_inst(tInstructionList *instrList);
void vypis(Table_item *T);
void vypis_inst_1(tInstructionList *instrList);
void vypis_ramec(frame *f);
/*
 *  provádění jednotlivých instrukcí
*/
int inter(tInstructionList *instrList)
{
	int tmp; // pomocná proměnná
	//FirstActive(instrList);
	//frame *ptr_stack; 
	//ptr_stack = NULL;


	tInst *II, *I;
	tInst inst;
//int i=1;

	//vypis_inst(instrList);
	//tmp = getchar();
	TargetActive(instrList, main_instr);
	while(1)
	{

		//char sss[50] = "ahoj";
		//char *dcx; 
		//SORT(sss);
		//printf("sort %d find %d\n", SORT, FIND);
		II = CurrentActive(instrList); // načtu instrukci k provedení
		//printf("run %p\t\t", (void*)II);
		//tmp = getchar();
		inst.instType = II->instType;
		inst.addr1 = II->addr1;
		inst.addr2 = II->addr2;
		inst.addr3 = (*II).addr3;
		I = &inst;
		//printf("%d\n", I->instType );
//vypis_inst_1(instrList);
		
		//if (ptr_stack != NULL) printf("chce to hledat v lokálním rámci\n");

		//vypis_ramec(ptr_stack);

		switch (I->instType) // interpretuju správnou instrukci
		{
			case I_STOP: //  == 0
			// instrukce konce programu
				//printf("pred koncem\n=====================\n");vypis_inst(instrList);  
				return 0;
				break;
			
			case  I_INT_TO_DOUBLE: // == 14
			// přetypování typu int na double (*addr3) = (double) (*addr1)

				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->add2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);			

				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				
				((Table_item*) I->addr3)->data->var.var_value->typ_real = 
						(double) ((Table_item*) I->addr1)->data->var.var_value->typ_integer;
				break;

			case I_DOUBLE_TO_INT: // == 15
			// přetypování typu double na int (*addr3) = (int) (*addr1)

				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	

				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_integer = 
						(int) ((Table_item*) I->addr1)->data->var.var_value->typ_real;
				break;

			case I_PRIRAD_INT:	// == 16
			// (*addr3) = (*addr1)
//vypis_inst_1(instrList);
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
//vypis_inst_1(instrList);
				
				if (!is_define((Table_item*)I->addr1)) { fprintf(stderr, "I_=_I\n"); return RUNTIME_ERROR_VAR; }
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_integer = 	
						((Table_item*) I->addr1)->data->var.var_value->typ_integer;
			//vypis_inst_1(instrList);

				break;
			case I_PLUS_INT: // == 17
			// (*addr3) = (*addr1) + (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) { fprintf(stderr, "I_+_I1\n"); return RUNTIME_ERROR_VAR ;}
				if (!is_define((Table_item*)I->addr2)) { fprintf(stderr, "I_+_I2\n"); return RUNTIME_ERROR_VAR;}
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_integer =
						((Table_item*) I->addr1)->data->var.var_value->typ_integer 
					+
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_MINUS_INT: // == 18
			// (*addr3) = (*addr1) - (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_integer =
						((Table_item*) I->addr1)->data->var.var_value->typ_integer 
					-
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_KRAT_INT: // == 19
			// (*addr3) = (*addr1) * (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_integer =
						((Table_item*) I->addr1)->data->var.var_value->typ_integer 
					*
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_DELENO_INT: // == 20
			// (*addr3) = (*addr1) / (*addr2) 
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				if (((Table_item*) I->addr2)->data->var.var_value->typ_integer == 0) 
				{ // dělení nulou 
					//error(RUNTIME_ERROR_DIVZERO); // se vyřeši asi až v main()
					// neuvažujene celočíselné dělení viz zadání - výsledek je vždy real(double)

				
					return (RUNTIME_ERROR_DIVZERO);
				}
				((Table_item*) I->addr3)->data->var.var_value->typ_real =
						((double) ((Table_item*) I->addr1)->data->var.var_value->typ_integer) 
					/
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_NOT_INT: // == 21
			// (*addr3) = - (*addr1)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_integer= 
				 	- ((Table_item*) I->addr1)->data->var.var_value->typ_integer;
				break;

			case I_ROVNO_INT: // == 22
			// (*addr3) = (*addr1) == (*addr2);
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = 
						((Table_item*) I->addr1)->data->var.var_value->typ_integer
					==
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_VETSI_INT: // == 23
			// (*addr3) = (*addr1) > (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = 
						((Table_item*) I->addr1)->data->var.var_value->typ_integer
					>
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;													

			case I_MENSI_INT: // == 24
			// (*addr3) = (*addr1) < (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 //vypis_inst_1(instrList);
				if (!is_define((Table_item*)I->addr1)) { fprintf(stderr, "I_<_I1\n"); return INTERPRET_ERROR; }
				if (!is_define((Table_item*)I->addr2)) { fprintf(stderr, "I_<_I2\n"); return INTERPRET_ERROR; }
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = 
						(((Table_item*) I->addr1)->data->var.var_value->typ_integer
					<
						((Table_item*) I->addr2)->data->var.var_value->typ_integer)?true:false;
//vypis_inst_1(instrList);				
				break;

			case  I_VETSIROVNO_INT: // == 25
			// (*addr3) = (*addr1) >= (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = 
						((Table_item*) I->addr1)->data->var.var_value->typ_integer
					>=
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_MENSIROVNO_INT: // == 26
			// (*addr3) = (*addr1) <= (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 			
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = 
						((Table_item*) I->addr1)->data->var.var_value->typ_integer
					<=
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_NEROVNO_INT: // == 27
			// (*addr3) = (*addr1) != (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = 
						((Table_item*) I->addr1)->data->var.var_value->typ_integer
					!=
						((Table_item*) I->addr2)->data->var.var_value->typ_integer;
				break;

			case I_PRIRAD_DOUBLE: // == 28
			// (*addr3) = (*addr1)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;

				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_real =
						((Table_item*) I->addr1)->data->var.var_value->typ_real;
				break;

			case I_PLUS_DOUBLE: // == 29
			// (*addr3) = (*add1) + (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_real = 
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					+
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_MINUS_DOUBLE: // == 30
			// (*addr3) = (*addr1) - (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_real = 
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					-
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_KRAT_DOUBLE: // == 31
			// (*addr3) = (*addr1) * (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_real = 
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					*
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;	

			case I_DELENO_DOUBLE: // == 32
			// (*addr3) = (*addr1) / (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				if (((Table_item*) I->addr2)->data->var.var_value->typ_real == 0) 
				{ // dělení nulou 
					//error(RUNTIME_ERROR_DIVZERO); // se vyřeši asi až v main()
				
					return (RUNTIME_ERROR_DIVZERO); // 8
				}

				((Table_item*) I->addr3)->data->var.var_value->typ_real = 
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					/
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;	

			case I_ROVNO_DOUBLE: // == 33
			// (*addr3) = (*addr1) == (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					==
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_VETSI_DOUBLE: // == 34
			// (*addr3) = (*addr1) > (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					>
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_MENSI_DOUBLE: // == 35
			// (*addr3) = (*addr1) > (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					<
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_VETSIROVNO_DOUBLE: // == 36
			// (*addr3) = (*addr1) >= (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					>=
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_MENSIROVNO_DOUBLE: // == 37
			// (*addr3) = (*addr1) <= (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					<=
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_NEROVNO_DOUBLE: // == 38
			// (*addr3) = (*addr1) != (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_real
					!=
						((Table_item*) I->addr2)->data->var.var_value->typ_real;
				break;

			case I_ROVNO_BOOLEAN: // == 46
			// (*addr3) = (*addr1) == (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);	
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_boolean
					==
						((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
				break;	

			case I_VETSI_BOOLEAN: // == 47
			// (*addr3) = (*addr1) > (*addr2) tzn (*addr1 == true && *addr2 == false)	
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_boolean
					>
						((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
				break;

			case I_MENSI_BOOLEAN: // == 48
			// (*addr3) = (*addr1) < (*addr2)	
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_boolean
					<
						((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
				break;

			case I_VETSIROVNO_BOOLEAN: // == 49
			// (*addr3) = (*addr1) >= (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_boolean
					>=
						((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
				break;			

			case I_MENSIROVNO_BOOLEAN: // == 50
			// (*addr3) = (*addr1) <= (*addr2)	
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_boolean
					<=
						((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
				break;

			case I_NEROVNO_BOOLEAN: // == 51
			// (*addr3) = (*addr1) != (*addr2)
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);	
				((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
						((Table_item*) I->addr1)->data->var.var_value->typ_boolean
					!=
						((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
				break;

			case I_PRIRAD_STRING: // == 39
			// StrCopyString zapouzdřuje strcpy nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 

				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				strInit(&((Table_item*) I->addr3)->data->var.var_value->typ_string);
//vypis_inst_1(instrList);


			//strInit(&((Table_item*) I->addr3)->data->var.var_value->typ_string);
//vypis_inst_1(instrList);


				tmp = strCopyString( 
					&(((Table_item*) I->addr3)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string) );
				//if (tmp == STR_SUCCESS) printf("snad ok prirad string\n");else printf("bohuzel\n");
//TODO: ošetřit návratovou hodnotu strCopyString
				break;

			case I_ROVNO_STRING: // == 40
			// StrCmpString zapouzdřuje strcmp nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				tmp = strCmpString(
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr2)->data->var.var_value->typ_string) );

				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = (tmp == 0);
				break;

			case I_VETSI_STRING: // == 41
			// StrCmpString zapouzdřuje strcmp nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				tmp = strCmpString(
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr2)->data->var.var_value->typ_string) );

				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = (tmp > 0);
				break;

			case I_MENSI_STRING: // == 42
			// StrCmpString zapouzdřuje strcmp nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				tmp = strCmpString(
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr2)->data->var.var_value->typ_string) );

				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = (tmp < 0);
				

				break;

			case I_VETSIROVNO_STRING: // == 43
			// StrCmpString zapouzdřuje strcmp nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				tmp = strCmpString(
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr2)->data->var.var_value->typ_string) );

				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = (tmp >= 0);
				break;

			case I_MENSIROVNO_STRING: // == 44
			// StrCmpString zapouzdřuje strcmp nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				tmp = strCmpString(
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr2)->data->var.var_value->typ_string) );

				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = (tmp <= 0);
				break;
					
			case I_NEROVNO_STRING: // == 45
			// StrCmpString zapouzdřuje strcmp nad typem string
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				tmp = strCmpString(
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr2)->data->var.var_value->typ_string) );

				((Table_item*) I->addr3)->data->var.var_value->typ_boolean = (tmp != 0);
				break;

			case I_SPOJIT_STRING: // == 52
			//StrCopyString(addr3, addr1); strcat(addr3, addr2)
			// můžem přidat do str.c/h 
				// velikost spojeného řetězce
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
		//	vypis_inst_1(instrList);
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
				strInit(&((Table_item*) I->addr3)->data->var.var_value->typ_string);
				tmp = 
						((Table_item*) I->addr1)->data->var.var_value->typ_string.length
					+
						((Table_item*) I->addr2)->data->var.var_value->typ_string.length;
				if (((Table_item*) I->addr3)->data->var.var_value->typ_string.allocSize  <= tmp+3)
				{// ne dostatek alokovaného místa 
	
					if (	( ((Table_item*) I->addr3)->data->var.var_value->typ_string.str = 
							
							(char*) realloc(((Table_item*) I->addr3)->data->var.var_value->typ_string.str, tmp + 5) ) 
						== 
							NULL)
         				return STR_ERROR; // nepodařilo se realokovat paměť
				}	

				((Table_item*) I->addr3)->data->var.var_value->typ_string.allocSize = tmp + 1;
				((Table_item*) I->addr3)->data->var.var_value->typ_string.length = tmp;
				strCopyString(
					&(((Table_item*) I->addr3)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string) );
				strcat(
					((Table_item*) I->addr3)->data->var.var_value->typ_string.str, 
					((Table_item*) I->addr2)->data->var.var_value->typ_string.str);
				((Table_item*) I->addr3)->data->var.var_value->typ_string.length = 
					strlen (((Table_item*) I->addr3)->data->var.var_value->typ_string.str);
//vypis_inst_1(instrList);

				break;
			case I_PRIRAD_BOOLEAN:
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);	
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);

				((Table_item *) I->addr3)->data->var.var_value->typ_boolean = 
					((Table_item*) I->addr1)->data->var.var_value->typ_boolean;
				break;
			case I_WRITE_INT:

				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
//vypis_inst_1(instrList);		
				//printf("%d", ((Table_item*) II->addr3)->data->var.var_value->typ_integer);
//vypis_inst_1(instrList);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
// vypis_inst_1(instrList);
			if (!is_define((Table_item*)I->addr3)) { fprintf(stderr, "I_W_I\n"); return RUNTIME_ERROR_VAR; }
			 
				printf("%d", ((Table_item*) I->addr3)->data->var.var_value->typ_integer);
				//vypis_inst_1(instrList);
				break;

			case I_WRITE_REAL: 
			// vypíšu pomocí printf s pomocí %g viz zadání
				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr3)) return RUNTIME_ERROR_VAR;
				printf("%g", ((Table_item*) I->addr3)->data->var.var_value->typ_real);
				break;

			case I_WRITE_BOOL:
				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 //vypis_inst_1(instrList);
				if (!is_define((Table_item*)I->addr3)) return RUNTIME_ERROR_VAR;
				printf("%s", ( ((Table_item*) I->addr3)->data->var.var_value->typ_boolean == true)?"TRUE":"FALSE");
				break;

			case I_WRITE_STRING:
				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr3)) return RUNTIME_ERROR_VAR;
				printf("%s", ((Table_item*) I->addr3)->data->var.var_value->typ_string.str==NULL?
								"":((Table_item*) I->addr3)->data->var.var_value->typ_string.str  );


				break;


			case I_READLN_STRING:
// I_PRIRAD_REAL,		
				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				set_define((Table_item*) I->addr3);	
				string s = {NULL, 999, 1000};
				s.str = malloc (1000*sizeof(char));
				fgets(s.str, 999, stdin);
				
				tmp = strlen(s.str);
				//printf("\nstrlen %d |%c|",tmp, s.str[tmp]);
				if (s.str[tmp-1] == EOF || s.str[tmp-1] == '\n'){
					s.str[tmp-1]='\0';
					s.length = tmp-2;
				}
				//printf("\nstrlen %d |%c|",tmp, s.str[tmp]);
				strCopyString(&((Table_item*) I->addr3)->data->var.var_value->typ_string, &s);
				free(s.str);

    			break;
   
    		case I_READLN_INT:
				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
    			set_define((Table_item*) I->addr3);
    			scanf("%d", & ((Table_item*) I->addr3)->data->var.var_value->typ_integer);
    			if (!isspace((tmp = getchar()))) return RUNTIME_ERROR_READ;
    			while(tmp != '\n' && tmp != EOF) tmp = getchar();
    			break;

    		
    		case I_READLN_REAL:
				//nastav_loc((Table_item *) I->addr1, ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
    			set_define((Table_item*) I->addr3);
    			scanf("%lf", &((Table_item*) I->addr3)->data->var.var_value->typ_real);
    			if (!isspace(tmp = (getchar()))) return RUNTIME_ERROR_READ;
    			while(tmp != '\n' && tmp != EOF) tmp = getchar();
    			break;

    		case I_LAB:
    		// pouze odpočinková instrukce :)
    			break;

    		case I_NOT: 
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
    			if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
    			set_define((Table_item*) I->addr3);
    			((Table_item*) I->addr3)->data->var.var_value->typ_boolean =
					((Table_item*) I->addr1)->data->var.var_value->typ_boolean == false ? true : false;

    			break;

    		case I_GOTO:

    			//TargetActive(instrList, I->addr3);
    			instrList->Active = (tItemList*) I->addr3;
    			break;


    		case I_IF_GOTO:
    			nastav_loc((Table_item **) &(I->addr1), ptr_stack);

    			if (((Table_item*) I->addr1)->data->var.var_value->typ_boolean != 0)
    				{instrList->Active = (tItemList*) I->addr3;}
    			break;
    
//PRIDANI VESTAVENÝCH FUNKCÍ
		case I_SORT_STRING:
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				
			if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
			set_define((Table_item*) I->addr3);

    			//vypis_inst_1(instrList);
    			
			strCopyString(
					&(((Table_item*) I->addr3)->data->var.var_value->typ_string),
					&(((Table_item*) I->addr1)->data->var.var_value->typ_string) );
			//vypis_inst_1(instrList);
			fSORT(
    				((Table_item*) I->addr3)->data->var.var_value->typ_string.str  );
    				//((Table_item *) I->addr3)->data->var.var_value->typ_string.length = 
    				//((Table_item *) I->addr1)->data->var.var_value->typ_string.length;
				//((Table_item *) I->addr3)->data->var.var_value->typ_string.allocSize = 
				//	((Table_item *) I->addr1)->data->var.var_value->typ_string.length;    				
    			//vypis_inst_1(instrList);
    		break	;
    //instrukce vyhledani podretezce
   		
		case I_FIND_STRING:
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);

  				((Table_item*) I->addr3)->data->var.var_value->typ_integer = fFIND(
  					((Table_item *) I->addr1)->data->var.var_value->typ_string.str,
  					((Table_item *) I->addr2)->data->var.var_value->typ_string.str	
  				);


  				break;
    		
    		case I_LENGTH:
			//vypis_inst_1(instrList);
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 
				if (!is_define((Table_item*)I->addr1)) return RUNTIME_ERROR_VAR;
				set_define((Table_item*) I->addr3);
    				((Table_item *) I->addr3)->data->var.var_value->typ_integer = fLENGTH(
    					((Table_item *) I->addr1)->data->var.var_value->typ_string.str
    				);
			//vypis_inst_1(instrList);	

    			break;

    		case I_COPY_POM: 
    			{
				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				//nastav_loc((Table_item *) I->addr3, ptr_stack);	
 
			

			if (!is_define((Table_item*)I->addr1)) { fprintf(stderr, "I_C_p1\n"); return RUNTIME_ERROR_VAR; }
			if (!is_define((Table_item*)I->addr2)) { fprintf(stderr, "I_C_p2\n"); return RUNTIME_ERROR_VAR; }
    			int tmp_copy1;
    			tmp_copy1 = ((Table_item *) I->addr1)->data->var.var_value->typ_integer;
    			int tmp_copy2; 
//printf("%d tmpcopy1\n", tmp_copy1);
    			tmp_copy2 = ((Table_item *) I->addr2)->data->var.var_value->typ_integer;
    			// potřebujeme 2 instrukce
//printf("active %d\n", instrList->Active->Data.instType); 

    			NextActive(instrList);//I_COPY
				II = CurrentActive(instrList);
				inst.instType = II->instType;
				inst.addr1 = II->addr1;
				inst.addr2 = II->addr2;
				inst.addr3 = (*II).addr3;
				I = &inst;

				nastav_loc((Table_item **) &(I->addr1), ptr_stack);
				//nastav_loc((Table_item *) I->addr2, ptr_stack);
				nastav_loc((Table_item **) &(I->addr3), ptr_stack);	
 			if (tmp_copy2 < 0 || tmp_copy1 <= 0) return RUNTIME_ERROR_OTHERS;
			if (!is_define((Table_item*)I->addr1)) { fprintf(stderr, "I_C_p3\n"); return RUNTIME_ERROR_VAR; }
			set_define((Table_item*) I->addr3);			
	//printf("active %d\n", instrList->Active->Data.instType);
	//vypis_inst_1(instrList);
			//((Table_item *) I->addr3)->data->var.var_value->typ_string.str = 
			//((Table_item *) I->addr3)->data->var.var_value->typ_string.str = malloc(tmp_copy2+4);
    			((Table_item *) I->addr3)->data->var.var_value->typ_string.str = fCOPY(
    				((Table_item *) I->addr1)->data->var.var_value->typ_string.str,
    				tmp_copy1,
    				tmp_copy2 );

    			((Table_item *) I->addr3)->data->var.var_value->typ_string.length = strlen (
    					((Table_item *) I->addr3)->data->var.var_value->typ_string.str );
				((Table_item *) I->addr3)->data->var.var_value->typ_string.allocSize = tmp_copy2+1;
//vypis_inst_1(instrList);				
	
		}
				break;	

			case I_CALL:
			// instrList->Active = addr1
			// addr2 - info o volané funkci 

				// vytvoř nový rámec		-push(frame)
				//printf("2 - ");vypis((Table_item*) I->addr2);printf("\n");
				//if (ptr_stack == NULL)printf("v I_CALL by nikdy nemel byt ptr_stack NULL :(\n");
			{		
				frame *pom_frame = malloc(sizeof(frame));

				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
				pom_frame->return_value = (Table_item*) I->addr2;
				pom_frame->return_Ins = instrList->Active;


				if (pom_frame == NULL) { printf("I_C\n"); return INTERPRET_ERROR; }
				if (ptr_stack == NULL){
					ptr_stack = pom_frame;
					ptr_stack->next_frame = NULL; 
				}else{
					pom_frame->next_frame = ptr_stack;
					ptr_stack = pom_frame;
				}

			}
				
				

				//skoč na začátek f-ce
				TargetActive(instrList, ((Table_item *)I->addr1)->data->func.instr);  

			break;

			case I_RETURN:
				// přiřazení návratové hodnoty
				// skok za I_CALL
				// pop
				//vypis_ramec(ptr_stack);
				if (!is_define(ptr_stack->next)) return RUNTIME_ERROR_VAR;
				set_define(ptr_stack->return_value);
				
				//jestli neni návratová hodnota o zanoření níž
					
 
				
				switch (ptr_stack->return_value->data->var.var_type)
				{
					case INTEGER:
						//printf("return- ");vypis(ptr_stack->return_value);printf("\n");
						//printf("hodnota- ");vypis(ptr_stack->next);printf("\n");
						ptr_stack->return_value->data->var.var_value->typ_integer = 
							ptr_stack->next->data->var.var_value->typ_integer;
						
						break;

					case REAL:
						ptr_stack->return_value->data->var.var_value->typ_real = 
							ptr_stack->next->data->var.var_value->typ_real;
						break;

					case BOOLEAN:
						ptr_stack->return_value->data->var.var_value->typ_boolean = 
							ptr_stack->next->data->var.var_value->typ_boolean;
						break;

					case STRING:
						ptr_stack->return_value->data->var.var_value->typ_string.str = 
							ptr_stack->next->data->var.var_value->typ_string.str;
						ptr_stack->return_value->data->var.var_value->typ_string.length = 
							ptr_stack->next->data->var.var_value->typ_string.length;
						ptr_stack->return_value->data->var.var_value->typ_string.allocSize = 
							ptr_stack->next->data->var.var_value->typ_string.allocSize;
						break;
					default: 
						fprintf(stderr, "nedostupny stav ajaj\n"); return INTERPRET_ERROR; break;
				}
				//instrList->Active = ptr_stack->return_Ins;
				//printf("I_RETURN instType %d\n",instrList->Active->Data.instType );
				TargetActive(instrList, ptr_stack->return_Ins);
				//printf("I_RETURN instType %d\n",instrList->Active->Data.instType );

				uvolni_seznam (ptr_stack->next); //uvolním lokální proměnné
				{
					frame * pom_f = ptr_stack;
					ptr_stack = ptr_stack->next_frame;
					free(pom_f);
				}
				

				break;

			case I_CREATE_RETURN:
				//printf("1 - ");vypis((Table_item*) I->addr1);printf("\n");
			{

				Table_item * pom_t;
				pom_t = malloc(sizeof(Table_item));
				if (pom_t == NULL) {fprintf(stderr, "I_C_R1\n"); return INTERPRET_ERROR;}
				pom_t->key = malloc(sizeof(string));
				if ((strInit(pom_t->key)) == STR_ERROR) { return INTERPRET_ERROR; }
				strCopyString(
							pom_t->key, 
							((Table_item *) I->addr1)->key );
				//pom_t->type = ((Table_item *) I->addr1)->type;
				pom_t->type = type_variable;

				pom_t->data = malloc(sizeof(uData));	
				if (pom_t->data == NULL) return INTERPRET_ERROR;
				pom_t->data->var.var_type = ((Table_item*) I->addr1)->data->var.var_type;
				if ( (pom_t->data->var.var_value = malloc(sizeof(uValue))) == NULL ) { return INTERPRET_ERROR;}
				if ( pom_t->data->var.var_type == STRING)
					if ((strInit(&pom_t->data->var.var_value->typ_string)) == STR_ERROR){ return INTERPRET_ERROR; }
				pom_t->next = NULL;
				ptr_stack->next = pom_t; 
			
				ptr_stack->next->next = parametry;
				parametry = NULL;
			}
				break;
			case I_CREATE_VAR:

			{

				Table_item * pom_t;
				pom_t = malloc(sizeof(Table_item));
				if (pom_t == NULL) {fprintf(stderr, "I_C_V1\n"); return INTERPRET_ERROR;}
				pom_t->key = malloc(sizeof(string));
				if ((strInit(pom_t->key)) == STR_ERROR) {  return INTERPRET_ERROR; }
				strCopyString(
							pom_t->key, 
							((Table_item *) I->addr1)->key );
				//pom_t->type = ((Table_item *) I->addr1)->type;
				pom_t->type = type_variable;

				pom_t->data = malloc(sizeof(uData));	
				if (pom_t->data == NULL) {fprintf(stderr, "I_C_V2\n"); return INTERPRET_ERROR;}
				pom_t->data->var.var_type = ((Table_item*) I->addr1)->data->var.var_type;
				if ( (pom_t->data->var.var_value = malloc(sizeof(uValue))) == NULL ) {  return INTERPRET_ERROR;}
				if ( pom_t->data->var.var_type == STRING)
					if ((strInit(&pom_t->data->var.var_value->typ_string)) == STR_ERROR) { return INTERPRET_ERROR;}
				pom_t->next = ptr_stack->next->next;
				ptr_stack->next->next = pom_t; 
			}
				break;

			case I_CREATE_PARAM: 
 			{
 				//nastav_loc((Table_item **) &(I->addr1), ptr_stack);
 				nastav_loc((Table_item **) &(I->addr2), ptr_stack);
 				if (!is_define((Table_item*)I->addr2)) return RUNTIME_ERROR_VAR;
				Table_item * pom_t;
				pom_t = malloc(sizeof(Table_item));
				if (pom_t == NULL) {  return INTERPRET_ERROR; }
				//pom_t->key = ((Table_item *) I->addr1)->key;
				//pom_t->type = ((Table_item *) I->addr1)->type;
				pom_t->type = type_variable;

				pom_t->key = malloc(sizeof(string));
				if ((strInit(pom_t->key)) == STR_ERROR) {  return INTERPRET_ERROR; }
				strCopyString(
							pom_t->key, 
							((Table_item *) I->addr1)->key );

				pom_t->data = malloc(sizeof(uData));	
				if (pom_t->data == NULL) {  return INTERPRET_ERROR; }
				pom_t->data->var.var_type = ((Table_item*) I->addr1)->data->var.var_type;
				if ( (pom_t->data->var.var_value = malloc(sizeof(uValue))) == NULL ) {  return INTERPRET_ERROR; }
				if ( pom_t->data->var.var_type == STRING)
					if ((strInit(&pom_t->data->var.var_value->typ_string)) == STR_ERROR) {  return INTERPRET_ERROR; }
				switch (pom_t->data->var.var_type)
				{
					case INTEGER:
						//printf("return- ");vypis(ptr_stack->return_value);printf("\n");
						//printf("hodnota- ");vypis(ptr_stack->next);printf("\n");
						pom_t->data->var.var_value->typ_integer = 
							((Table_item*) I->addr2)->data->var.var_value->typ_integer;
						
						break;

					case REAL:
						pom_t->data->var.var_value->typ_real = 
							((Table_item*) I->addr2)->data->var.var_value->typ_real;
						break;

					case BOOLEAN:
						pom_t->data->var.var_value->typ_boolean = 
							((Table_item*) I->addr2)->data->var.var_value->typ_boolean;
						break;

					case STRING:
						strCopyString(
							&pom_t->data->var.var_value->typ_string, 
							&(((Table_item *) I->addr2)->data->var.var_value->typ_string)  );
						break;
					default: 
						fprintf(stderr, "nedostupny stav ajaj\n"); return INTERPRET_ERROR; break;
				}
				pom_t->data->var.local = 1;
				set_define(pom_t);
				//vypis(pom_t);printf("\n");
				pom_t->next = NULL;
				if (parametry == NULL)
					parametry = pom_t;
				else{ 	
					pom_t->next = parametry;
					parametry = pom_t;
				}
				pom_t = NULL;
			}

				break;
//



			default: 
				fprintf(stderr, "instrukci %d jeste nemam\n", I->instType );

		} //switch



		//vypis_inst_1(instrList);
		NextActive(instrList); // posunu se v seznamu instrukcí

		
	}// while(1)



}// instr()
void vypis(Table_item *T){

	if (T == NULL){printf("t == NULL"); return;}
	printf("t == %p ", (void *)T);
	if (T->type == DEFINE || T->key->str[0] == '$' ) {printf(" DEFINE ");} else {printf(" NEdefine "); }
	if (T->key == NULL){printf("t->key == NULL "); return;}else{printf("t->key == %s ",T->key->str);}
	if (T->type == type_function) {printf(" F-CE "); return;}
	if (T->data == NULL){printf("t->data == NULL"); return;}else{printf("t->data == OK ");}
	printf(" T->data == %p ", (void*)T->data);

	if (T->data->var.var_type == BOOLEAN){printf("BOOLEAN value=%d ", T->data->var.var_value->typ_boolean);return; }
	
	if (T->data->var.var_type == INTEGER){printf("INTEGER value=%d ", T->data->var.var_value->typ_integer);printf("value == %p ", (void *)T->data->var.var_value);return; }
	if (T->data->var.var_type == REAL){printf("REAL value=%g ", T->data->var.var_value->typ_real);return;}
	if (T->data->var.var_type == STRING){printf("STRING l=%d a=%d ", T->data->var.var_value->typ_string.length,
																	T->data->var.var_value->typ_string.allocSize); 
		if(T->data->var.var_value->typ_string.str == NULL) {printf("str == NULL " );} 
		else {printf("str == %s ", T->data->var.var_value->typ_string.str);}}

}
// pomocná funkce na určení jestli má danná proměnná definovanou hodnotu
int is_define(Table_item * t){
	if (t->key->str[0] == '$') return true;
	if (t->type == DEFINE) return true;
	return false;

}

void set_define(Table_item *t){
	t->type = DEFINE;
}

void nastav_loc(Table_item **t, frame * p){
	if (p == NULL) return;
	Table_item *pom = p->next;
	//printf(">>>HLED8M<<<\n");
	while (pom != NULL){
		if (strCmpString((*t)->key, pom->key)  == 0 )// jsou stejné
		{
			//printf(">>>NASEL JSEM×××<<<\n");
			*t = pom;
			return;
		}
		pom = pom->next;
	}
}

void pom(int i, Table_item *t){
	if (t == NULL){ printf("%d t == NULL", i);}
	printf("%d %s %d uk=%s alloc %d\n", i, t->key->str, t->data->var.var_type, (t->data->var.var_value == NULL)?"NULL:(": "neNULL:)", t->data->var.var_value->typ_string.allocSize);	
} 

void vypis_inst(tInstructionList *instrList){
	tItemList* f = instrList->First;	
	while(f != NULL){
		/*
		printf("1a- ");vypis((Table_item*) f->Data.addr1);printf("\n");
		printf("2a - ");vypis((Table_item*) f->Data.addr2);printf("\n");
		printf("3a - ");vypis((Table_item*) f->Data.addr3);printf("\n");
*/
		printf("%d <= instrukce  %p \n", f->Data.instType, (void *)f);
		if (f->Data.instType == 0){printf("---------------\n"); return;}
		f = f->NextItem;



	} 
	printf("-----konec instrukcí----");

}

void vypis_inst_1(tInstructionList *instrList){
	tItemList* f = instrList->Active;	
	
		printf("<<<<<<<<<<<<<<<%d <= instrukce>>>>>>>>>>>>>\n", f->Data.instType);
		printf("1 - ");vypis((Table_item*) f->Data.addr1);printf("\n");
		printf("2 - ");vypis((Table_item*) f->Data.addr2);printf("\n");
		printf("3 - ");vypis((Table_item*) f->Data.addr3);printf("\n");

} 


int fLENGTH(char *s){
	return strlen(s);
}

char * fCOPY(char *s, int i, int n){
	//printf("copy\n");
	int pom;
	pom = strlen(s);

	char *c = malloc((n+1)* sizeof(char));
	c[0] = '\0';
	if (n == 0) return c;
	//printf("copy\n");
	if (c == NULL) { fprintf(stderr, "fcopy if\n"); return NULL ;}

	//*strncpy (char *dest, char  *src, int n)
	strncpy(c, &(s[i-1]), n);
	if (pom < i) c[0] = '\0';
	if (pom < i+n-1) c[pom-i+1] = '\0';
	c[n] = '\0';
	
	return c;
}
void vypis_ramec(frame *f){
	if (f == NULL){printf("--------prazdny zasobnik-------\n"); return;}
	Table_item * pom = f->next;
	printf("--------lllllllllll---------\n");
	while(pom != NULL){
		printf("loc - rámec: ");vypis(pom);printf("\n");
		pom = pom->next;
	}
	printf("--------kkkkkkkkkkk----------\n");
}

void uvolni_seznam (Table_item * t){
    Table_item *item;
    Table_item *last;

    item = t;
    while (item != NULL) {
      last = item;
      item = item->next;
      if (last->type == type_variable || last->type == DEFINE) {
        free(last->data->var.var_value);
      }

      free(last->key->str);
      free(last->key);
      // mozno bude treba uvolnovat aj data->...
      free(last->data);
      free(last);
    }
}