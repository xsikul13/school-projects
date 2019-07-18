/************************************************
* Projekt: Implementace interpretu jazyka IFJ14
* Varianta: b/1/II (tým 026)
*
* Soubor: scaner.c
* Autor: Měřínský Josef
*
* Tým:  Motlík Matúš (vedoucí)      xmotli02
*       Marcin Juraj                xmarci05
*       Mour Lukáš                  xmourl00
*       Měřínský Josef              xmerin02
*       Šikula Vojtěch              xsikul13
************************************************/

/* Lexikalni analyzator */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "str.h"
#include "scaner.h"
#include "errors.h"

FILE *source;
int current_line = 1;

t_token *token_alloc() {
  t_token *token;

  if ((token = malloc ( sizeof(t_token))) == NULL) {
    fprintf(stderr, "Malloc error\n");
    return NULL;
  }

  if ((token->atribut = malloc ( sizeof(U_attr))) == NULL) {
    fprintf(stderr, "Malloc error\n");
    free(token);
    return NULL;
  }

  if (string_alloc(token) != OK)
    return NULL;

  return token;
}

int string_alloc(t_token *token) {
  if ((token->atribut->t_STRING = malloc ( sizeof(string) )) == NULL) {
      fprintf(stderr, "Malloc error\n");
      free(token);
      free(token->atribut);
      return RUNTIME_ERROR_OTHERS;
    }

    if ((token->atribut->t_STRING->str = (char*) calloc((30), sizeof(char))) == NULL) {
      fprintf(stderr, "Malloc error\n");
      free(token);
      free(token->atribut);
      free(token->atribut->t_STRING);
      return RUNTIME_ERROR_OTHERS;
    }
    token->atribut->t_STRING->length = 30;
    token->atribut->t_STRING->allocSize = 30*sizeof(char);

    return OK;
}

/**
 * Funkcia uvolni token
 */
void token_free(t_token *token) {
  if (token->stav != INTEGER_NUMB && token->stav != DOUBLE_NUMB) {
    free(token->atribut->t_STRING->str);
    free(token->atribut->t_STRING);
  }
  free(token->atribut);
  free(token);
}

void setSourceFile(FILE *f)
{
  source = f;
}

/* Funkce pro kontrolu stringu */

int StringControl (string *str, t_token *token)
{ 
	char c;
	int iPas = 1;
	int Pom = 0;
	int lengt = (str->length) - 2;
	string final_string;
	strInit(&final_string);
	while(iPas <= lengt)
	{
		Pom = 0;
		c = str->str[iPas];
		if(c == 39)
		{
			iPas++;
			c = str->str[iPas];
			if(c == 39)
			{
				if(strAddChar(&final_string,c)==1)
				{
					strFree(&final_string);
					return RUNTIME_ERROR_OTHERS;
				}	
				iPas++;
			}
			else if(c == '#')
			{
				iPas++;
				c = str->str[iPas];
				while(c != 39)
				{
					Pom = Pom*10;
					Pom+= c-48;
					iPas++;
					c = str->str[iPas];
				}
				iPas++;
				if (Pom > 255)
				{
					strFree(&final_string);
					return LEX_ERROR;
				}
				else
				{
					if(strAddChar(&final_string, Pom) == 1)
					{
						strFree(&final_string);
						return RUNTIME_ERROR_OTHERS;
					}
				}					
			}
			else
			{
				strFree(&final_string);
				return LEX_ERROR;
			}
		}
		else if(c > 31)
		{
			if(strAddChar(&final_string,str->str[iPas]) == 1)
			{
				strFree(&final_string);
				return RUNTIME_ERROR_OTHERS;
			}
			iPas++;
		}
		else
		{
			strFree(&final_string);
			return LEX_ERROR;
		}
	}
	if(strAddChar( &final_string,'\0') == 1 )
	{
		strFree(&final_string);
		return RUNTIME_ERROR_OTHERS;
	}
	strCopyString(str,&final_string);

	strFree(&final_string);

	token->stav = C_STRING;
	return OK;
}

int getNextToken(t_token *token) 
/* Hlavni funkce lexikalniho analyzatoru*/
{
	if (token->stav == INTEGER_NUMB || token->stav == DOUBLE_NUMB) 
	{
		if (string_alloc(token) != OK)
			return RUNTIME_ERROR_OTHERS;
	}
	strClear(token->atribut->t_STRING);

	int state = FIRST_STATE;
	int c;
	while (1)
	{
		c = fgetc(source);
		switch (state)
		{
			case FIRST_STATE:
			/* ----------Pocatecni stav automatu---------- */
				// bile znaky budeme ignorovat
				if (isspace(c)) {
					state = FIRST_STATE;
					if (c == '\n') {
						current_line++;
					}
				}
				// komentar zacinajici '{'
				else if (c == '{')
					state = COMMENT;
				// klicove slovo nebo identifikator
				else if ( (isalpha(c)) || (c == '_') )
				{
					c = tolower(c);
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = KWorID;				}	
				// pro string
				else if (c == 39)					
				{			
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = STRING_STATE;
				}
				// pro cisla
				else if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = NUMB_STATE;
				}
				// za '<' muze nasledovat '=' nebo '>'
				else if (c == '<')
					state = SMALL_STATE;
				// za '>' muze nasledovat '='
				else if (c == '>')
					state = BIG_STATE;
				// za ':' muze nasledovat '='
				else if (c == ':')
					state = ASSIGN_STATE;
				else if (c == ';') 
				{
					token->stav = SEMICOLON;
					return OK;
				}
				else if (c == '+') 
				{
					token->stav = PLUS;
					return OK;
				}	
				else if (c == '-') 
				{
					token->stav = MINUS;
					return OK;
				}	
				else if (c == '*') 
				{
					token->stav = KRAT;
					return OK;
				}	
				else if (c == '/') 
				{
					token->stav = DELENIE;
					return OK;
				}		
				else if (c == '(')
				{
					token->stav = LEFT_BRACKET;
					return OK;
				}	
				else if (c == ')') 
				{
					token->stav = RIGHT_BRACKET;
					return OK;
				}
				else if (c == ',') 
				{
					token->stav = COMMA;
					return OK;
				}
				else if (c == '[') 
				{
					token->stav = RIGHT_BRACKET2;
					return OK;
				}	
				else if (c == ']') 
				{
					token->stav = LEFT_BRACKET2;
					return OK;
				}
				else if (c == '=') 
				{
					token->stav = EQUAL;
					return OK;
				}
				else if (c == EOF) 
				{
					token->stav = END_OF_FILE;
					return OK;
				}
				else
					return LEX_ERROR;

			break;

			case COMMENT:
			/* ----------Komentar---------- */
				if (c =='}') 
					state = FIRST_STATE;
				// Pokracovani libovolnymi znaky, ovsem kontroluje pokud prijde EOF
				else if (c == EOF)
					return LEX_ERROR;
			break;

			case KWorID:
			/* ----------Identifikator nebo klicove slovo---------- */
				if ((isalnum(c)) || (c == '_'))
				{
					c = tolower(c);
					if(strAddChar(token->atribut->t_STRING, c) == 1)		// dokud mame alfanumerick znak 
						return RUNTIME_ERROR_OTHERS;		
				}
				else 
				// Konec identifikatoru
				{
					// Kontrola nalezeni klicovych slov, ktere se nesmi nachayet jako ID
					ungetc(c, source);
					if (strCmpConstStr(token->atribut->t_STRING, "begin") == 0)
					{
						token->stav = BEGIN;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "boolean") == 0)
					{
						token->stav = BOOLEAN;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "do") == 0)
					{
						token->stav = DO;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "else") == 0)
					{
						token->stav = ELSE;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "end") == 0)
						state = END_STATE;
					else if (strCmpConstStr(token->atribut->t_STRING, "false") == 0)
					{
						token->stav = FALSE_KW;
						return OK;
					}		
					else if (strCmpConstStr(token->atribut->t_STRING, "find") == 0)
					{
						token->stav = FIND;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "forward") == 0)
					{
						token->stav = FORWARD;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "function") == 0)
					{
						token->stav = FUNCTION;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "if") == 0)
					{
						token->stav = IF;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "integer") == 0)
					{
						token->stav = INTEGER;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "readln") == 0)
					{
						token->stav = READLN;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "real") == 0)
					{
						token->stav = REAL;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "sort") == 0)
					{
						token->stav = SORT;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "string") == 0)
					{
						token->stav = STRING;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "then") == 0)
					{
						token->stav = THEN;
						return OK;
					}		
					else if (strCmpConstStr(token->atribut->t_STRING, "true") == 0)
					{
						token->stav = TRUE_KW;
						return OK;
					}	
					else if (strCmpConstStr(token->atribut->t_STRING, "var") == 0)
					{
						token->stav = VAR;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "while") == 0)
					{
						token->stav = WHILE;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "repeat") == 0)
					{
						token->stav = REPEAT;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "until") == 0)
					{
						token->stav = UNTIL;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "write") == 0)
					{
						token->stav = WRITE;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "copy") == 0)
					{
						token->stav = COPY;
						return OK;
					}
					else if (strCmpConstStr(token->atribut->t_STRING, "length") == 0)
					{
						token->stav = LENGTH;
						return OK;
					}
					else
					// Jedna se o identifikator	
					{
						token->stav = ID;
						return OK;
					}
				}
			break;	

			case STRING_STATE:
			/* ----------Stringy ----------*/
				if(c == 39)		
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = NEXT_STRING;
				}
				else if (c > 31)
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1) // pokracovani v nacitani znaku
						return RUNTIME_ERROR_OTHERS;	
				}
				else
					return LEX_ERROR;
			break;

			case NUMB_STATE:
			/* ---------- Cisla ---------- */
				if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
				}
				else if (c == '.')		// desetina carka (double)
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = DOUBLE_COMMA;
				}
				else if (c == 'e' || c == 'E')		// exponencialni cislo (double)
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = DOUBLE_EXP;
				}
				else
				{
					ungetc(c, source);
					// Ulozeni stavu a attributu do tokenu a nasledne uvolneni kvuli zmene typu
					 
					char *ptr;
					long l = strtol(token->atribut->t_STRING->str, &ptr,10);
					if (*ptr != '\0' || l<0 || l>INT_MAX)
						return LEX_ERROR;

					free(token->atribut->t_STRING->str);
					free(token->atribut->t_STRING);

					token->atribut->t_INTEGER = l;
					token->stav = INTEGER_NUMB;
					return OK;
				}
				
			break;

			case SMALL_STATE:
			/* ----------Operator <= ; <> ---------- */
				if (c == '=')
					token->stav = SMALL_OR_EQUAL;	// mensi nebo roven
				else if (c == '>')
					token->stav = NOT_EQUAL;		// neni roven
				else
				{
					ungetc(c, source);
					token->stav = SMALL;			// je mensi
				}
				return OK;
			break;

			case BIG_STATE:
			/* ----------Operator >= ---------- */
				if (c == '=')
					token->stav = BIG_OR_EQUAL;	// je vetsi nebo rovno
				else
				{
					ungetc(c, source);
					token->stav = BIG;				// je vetsi
				}
				return OK;
			break;

			case ASSIGN_STATE:
			/* ----------Operator := ---------- */
				if (c == '=')	
					token->stav = ASSIGN; 				// prirad
				else
				{
					ungetc(c, source);
					token->stav = COLON; 			// Dvojtecka
				}
				return OK;
			break;

			case END_STATE:
			/* ---------- END/END. ---------- */

				if (c == '.')
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)		
						return RUNTIME_ERROR_OTHERS;	
					token->stav = END_OF_PROGRAM;
				}	
				else
				{
					ungetc(c, source);
					token->stav = END; 			
				}
				return OK;
			break;

			case DOUBLE_COMMA:
			/* ---------- Double - Desetina carka ---------- */
				if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = DOUBLE_COMMA_NEXT;
				}
				else
					return LEX_ERROR;
			break;

			case DOUBLE_EXP:
			/* ---------- Double - exponent ---------- */
				if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = DOUBLE_EXP_NEXT;
				}
				else if (c == '+' || c == '-')		// za e/E se muze take vyskytnout + - 
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = EXP_CONTROL_STATE;
				}
				else
					return LEX_ERROR;
			break;

			case DOUBLE_COMMA_NEXT:
			/* ---------- Co se muze vyskytnout za desetinou carkou ---------- */
				if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
				}
				else if (c == 'e' || c == 'E')		// za desetinou carkou opet muze byt exponent
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = DOUBLE_EXP;						// pokracujem na exponenta
				}
				else
				{
					ungetc(c, source);
								

					char *ptr;
					double d = strtod(token->atribut->t_STRING->str,&ptr);	// následně uložení do t_integer prez promennou
					if (*ptr != '\0')
						return LEX_ERROR;

					free(token->atribut->t_STRING->str);
					free(token->atribut->t_STRING);

					token->atribut->t_REAL = d;
					token->stav = DOUBLE_NUMB;
					return OK;
				}
			break;

			case DOUBLE_EXP_NEXT:
			/* ---------- Double - cast za exponentem ---------- */
				if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)	// nacitame do konce
						return RUNTIME_ERROR_OTHERS;		
				}
				else
				{
					ungetc(c , source);
					
					char *ptr;
					double d = strtod(token->atribut->t_STRING->str,&ptr);	// následně uložení do t_integer prez promennou
					if (*ptr != '\0')
						return LEX_ERROR;

					free(token->atribut->t_STRING->str);
					free(token->atribut->t_STRING);

					token->atribut->t_REAL = d;
					token->stav = DOUBLE_NUMB;		
					return OK;			
				}
			break;

			case EXP_CONTROL_STATE:
			/* ---------- Kontrola exponencialni casti ---------- */
				if(isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = DOUBLE_EXP_NEXT;
				}
				else
				{
					ungetc(c, source);
					return LEX_ERROR;
				}
			break;
				
			case NEXT_STRING:
			/* ---------- pokracovani retezce ---------- */
				if(c == 39)		
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = STRING_STATE;		
				}			
				else if (c == '#')		// zapisovani ostatnich znaku pomoci escape sekvence
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = ESCAPE_STATE;
				}
				else
				{
					ungetc(c, source);
					return StringControl(token->atribut->t_STRING,token);
				}
			break;

			case ESCAPE_STATE:
			/* ---------- Escape sekvence ---------- */
				if (c == '0')			// znaky v escape sekvenci jsou od 1 - 255
					state = ESCAPE_STATE;			// a přebytečné počáteční nuly jsou ignorovány
				else if (isdigit(c))
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = ESCAPE_STATE2;
				}
				else if (c == 39)
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = STRING_STATE;
				}
				else
					return LEX_ERROR;	
			break;

			case ESCAPE_STATE2:
			/* ---------- Escape sekvence 2 ---------- */
				if(isdigit(c)) 		// 1 - 255
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
				}
				else if (c == 39)
				{
					if(strAddChar(token->atribut->t_STRING, c) == 1)
						return RUNTIME_ERROR_OTHERS;
					state = STRING_STATE;
				}
				else
					return LEX_ERROR;
			break;
		}
	
	}
	return OK;
}