/*
 * Soubor:  proj1.c
 * Datum:   2013/11/14
 * Autor:   Vojtech Sikula, xsikul13@stud.fit.vutbr.cz
 * Projekt: Pocitani slov, projekt č. 1 pro predmet IZP
 * Popis:   Program pocita slova v zadanem textu podle zadanych argumentu.
 *          Pro vice informaci: ./proj1 --help
 */

#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <limits.h>
#define MAXSLOVO 80 // maximalni delka vypisovaneho slova

int umisteni_a(char *);             //vraci zadane umisteni
int hledany_znak_a(char *);         //vraci 0 je-li v argumentu zadan spatny znak
void printHELP();                   //tiskne napovedu
int je_hledany(int, char);          // vraci 1 jedna-li se o platny znak, jinak vraci 0
int je_znak(int);                   // vraci 2 jedla-li se o hledany znak, jinak vraci 0
int nastav_a(int, char *[],int *, int *, char *); // nastaveni promenych z argumentu -  (int argc, char *argv[], *int hledany_znak, int* umisteni, char* debug)
void chyby(int); // vypsani chybove hlasky
///////////////////////////////////////////////////////////////////////////////
/**
 * Hlavní program
 */
int main(int argc, char *argv[])
{
    int hledany_znak; // hledany znak
    int umisteni = 0; //umisteni hledaneho znaku
    char debug =0; //debug == 0 - vypnut debug rezim, debug == 1 - zapnut debug rezim
    int znak; // pro nacitani znaku
    int pocet_slov = 0; // pocet nalezenych slov
    int i=0; // indexace znaku ve slove
    int zapocteno = 0; // urcuje jestli bylo dane slovo zapocteno
    int chyba = 0; // cislo chyby
////////////////////////////////////////////////////////////////////////////////
    if (argc == 2 && strcmp(argv[1],"--help")==0)
    {
        printHELP();
        return 0;
    }
    chyba = nastav_a(argc, argv, &hledany_znak, &umisteni, &debug);
    if (chyba)
    {
        chyby(chyba);
        return chyba;
    }
////////////////////////////////////////////////////////////////////////////////
    while ((znak = getchar())!= EOF)
    {
        if (je_znak(znak))
        {
            //jedna se o validni znak
            i++;
            if (debug && i <= MAXSLOVO)
            {
                //pokud je nastaven debug rezim a slovo je kratsi nez MAXSLOVO
                putchar(znak);
            }
            if (je_hledany(znak, hledany_znak) && (umisteni == i || (umisteni == 0 && zapocteno == 0)))
            {
                pocet_slov++;
                zapocteno = 1;
            }
        }else{
            //nejedna se o validni znak
            if (i != 0 && debug)
            {
                putchar('\n');
            }
            i = 0;
            zapocteno = 0;
        }
    }
    printf("%d\n",pocet_slov);
}  // main()

/**
 * Tisk Napovedy
 */
void printHELP()
{
printf(
"\n -----------------------------------------------------------------------------\n"
" -----------------------------------NAPOVEDA----------------------------------\n"
" -----------------------------------------------------------------------------\n"
"Program Pocitani slov spocita pocet slov na stupu podle zadanych argumentu."
"Program se spousti v nasledujici podobe\n"
"       (./proj1 znaci umisteni a nazev programu\n"
"       a hranate zavorky reprezentuji volitelnost daneho argumentu programu):\n"
"./proj1 --help\n"
"nebo\n"
"./proj1 X [N] [-d]\n"
"Argumenty programu:\n"
"--help zpusobi, ze program vytiskne napovedu pouzivani programu a skonci.\n"
"X reprezentuje hledany znak v pocitanych slovech.\n"
"       Specialni znak ':' navic symbolizuje libovolne cislo 0-9, \n"
"       znak '^' symbolizuje libovolne velke pismeno A-Z\n"
"       a znak '.' symbolizuje libovolny znak.\n"
"Volitelny argument N predstavuje cislo vyjadrujici pozici hledaneho znaku v pocitanych slovech.\n"
"       Platne pocitane slovo musi mit hledany znak prave na dane pozici.\n"
"       Cislo N je kladne - prvni znak ve slove ma pozici 1.\n"
"Volitelny argument -d zapina tzv. ladici (debug) rezim programu.\n"
"       V ladicim rezimu program na vystup prubezne vypisuje kazde nactene slovo, jedno slovo na radek.\n"
"       Pocet slov je programem vypsan teprve po skonceni pocitani.\n"
" -----------------------------------------------------------------------------\n"
"autor: Vojtech Sikula (c) 2013\n"
"e-mail: xsikul13@stud.fit.vutbr.cz\n"
" -----------------------------------------------------------------------------\n"
);
}
/**
 * Zjisteni jestli parametru programu byl zadan spravny znak. V kladnem pripade ho vrati, jinak vraci 0.
 */
int hledany_znak_a(char *znak)
{
    if (znak[1] == '\0') // jestli se jedna o jedno znakovy retezec
        {
            return ((
                        (('a' <= znak[0])&&( znak[0] <= 'z')) ||
                        (('A' <= znak[0])&&( znak[0] <= 'Z')) ||
                        (('0' <= znak[0])&&( znak[0] <= '9')) ||
                        ('.' == znak[0]) ||
                        (':' == znak[0]) ||
                        ('-' == znak[0]) ||
                        ('_' == znak[0]) ||
                        ('^' == znak[0])
                        )? znak[0]:0);
        }else{
            return  0;
        }
}
/**
 * Zjisteni jestli parametr programu byl zadan cislicovy udaj. V kladnem pripade ho prevede na cislo, jinak vrati 0.
 */
int umisteni_a(char *ret)
{
    int umisteni = 0, dalsi;
    int i = 0;
    while (ret[i] != '\0')
    {
        if (ret[i] < '0' || '9' < ret[i])
        {
            return 0;
        }
        if ((dalsi=(umisteni)*10+(ret[i] -'0'))<umisteni) // jestli pretece
         {
            return 0;
         }
        umisteni = dalsi;
        i++;
    }
    return (int)umisteni;
}
/**
 * Zjisteni jestli se jedna o znak. V kladnem pripade vraci 1, jinak 0.
 */
int je_znak(int znak)
{
    if (
        (znak <='9'&& '0'<= znak)||(znak <='Z'&& 'A'<= znak)||(znak <='z'&& 'a'<= znak)||
        ('-' == znak)||('_' == znak))
    {
        return 1;
    }else{
        return 0;
    }
}
/**
 * Zjisteni zda se jedna o hledany znak. V kladnem pripade vraci 1, jinak 0.
 */
int je_hledany(int znak, char hledany)
{
    if (hledany == '.')
    {
        return 1;
    }
    else if(hledany == ':')
    {
        return (znak <='9'&& '0'<= znak)?1:0;
    }
    else if(hledany == '^')
    {
        return (znak <='Z'&& 'A'<= znak)?1:0;
    }
    else if (hledany == znak)
    {
        return 1;
    }else{
        return 0;
    }
}
/**
 * Nastaveni promennych podle argumentu. V pripade spatnych argumentu vrati cislo chyby, jinak 0.
 */
int nastav_a(int argc, char *argv[], int *hledany_znak, int* umisteni, char* debug)
{
     if (argc < 2 || 4 < argc ) // spusteno s nespravnym poctem argumentu
    {
            return 1;

    }
    if (argc == 2) // dva argumenty tzn. jen specifikace hledaneho znaku || --help
    {
        if ((*hledany_znak = hledany_znak_a(argv[1]))== 0)
        {
            return 2;
        }
    }
    if (argc == 3) // specifikace poradi znaku nebo debug rezim
    {
        if ((*hledany_znak = hledany_znak_a(argv[1]))== 0)
        {
          return 2;
        }
        *debug = (strcmp(argv[2], "-d")?0:1);
        if (*debug != 1)
        {
            if ((*umisteni = umisteni_a(argv[2])) == 0)
            {
            return 3;
            }
        }
    }
    if (argc == 4)
    {
         if ((*hledany_znak = hledany_znak_a(argv[1]))== 0)
        {
            return 2;
        }
        if ((*umisteni = umisteni_a(argv[2])) == 0)
        {
            return 3;
        }
        *debug = (strcmp(argv[3], "-d")?0:1);
        if (!(*debug))
        {
            return 4;
        }
    }
    return 0;
}
/**
 * Vypise napovedu a chybovou hlasku
 */
void chyby(int chyba)
{
    char *errors[] = {
        "CHYBA - spusteno se spatnym poctem argumentu\n",
        "CHYBA - spatne zadan prvni argument\n",
        "CHYBA - spatne zadan druhy argument\n",
        "CHYBA - spatne zadan treti argument\n"
    };

    printHELP();
    fprintf(stderr,"%s", errors[chyba-1]);
}
