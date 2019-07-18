/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: scaner.h
* Autor: Měřínský Josef
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/	

#ifndef SCANER_H
#define SCANER_H

#include "ial.h"

extern int current_line;

/* Identifikator */
#define ID 			21	

/* Klicova slova */
#define BEGIN			22
#define READLN      23
#define DO				24
#define ELSE			25
#define END				26
#define END_OF_PROGRAM		27
#define FALSE_KW		28
#define FIND			29
#define FORWARD			30
#define FUNCTION		31
#define IF 				32
#define INTEGER			33
#define BOOLEAN     34
#define REAL 			35
#define SORT 			36
#define STRING			37
#define THEN			38
#define TRUE_KW			39
#define VAR 			40
#define WHILE 			41
#define REPEAT      87
#define UNTIL       88
#define WRITE 			42

/* Others */
#define SMALL_OR_EQUAL	43
#define NOT_EQUAL		44
#define SMALL 			45
#define BIG_OR_EQUAL 	46
#define BIG 			47
#define ASSIGN 			48
#define COLON 			49
#define SEMICOLON		50
#define LEFT_BRACKET		51
#define RIGHT_BRACKET		52
#define COMMA			53
#define LEFT_BRACKET2		54
#define RIGHT_BRACKET2		55

#define PLUS			56
#define MINUS			57
#define KRAT			58
#define DELENIE			59
#define EQUAL 			60

/* Typy */
#define INTEGER_NUMB	61	
#define DOUBLE_NUMB 	62
#define C_STRING		63

#define COPY 64
#define LENGTH 65

/* EOF*/
#define END_OF_FILE		90

/* STAVY case*/
#define FIRST_STATE 	70
#define COMMENT			71
#define KWorID			72
#define	STRING_STATE	73
#define	NUMB_STATE 		74
#define	SMALL_STATE		75
#define	BIG_STATE		76
#define	ASSIGN_STATE	77
#define END_STATE		78
#define DOUBLE_COMMA	79
#define DOUBLE_EXP		80
#define DOUBLE_COMMA_NEXT	81
#define DOUBLE_EXP_NEXT		82
#define EXP_CONTROL_STATE	83
#define NEXT_STRING			84
#define ESCAPE_STATE		85
#define ESCAPE_STATE2		86

/* unia pre ulozenie hodnot rozneho typu */
typedef union {
  int t_INTEGER;
  double t_REAL;
  string *t_STRING;
}U_attr;

/* struktura pre ulozenie tokenu a jeho atributu */
typedef struct struct_token {
    int stav;
    U_attr *atribut;
} t_token;

/* Hlavicka funkce */
void setSourceFile(FILE *f);
int getNextToken(t_token *token);
void token_free(t_token *token);
t_token *token_alloc();
int string_alloc(t_token *token);

#endif
