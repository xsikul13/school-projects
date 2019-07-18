/*
 * Soubor:  proj3.c
 * Datum:   2013/11/25
 * Autor:   Vojtech Sikula, xsikul13@stud.fit.vutbr.cz
 * Projekt: Hledani obrazcu, projekt c. 3 pro predmet IZP
 * Popis:   Program podle zadanych prarametru a souboru zjisti,
 *          validnost souboru, nejdelsi usecku nebo ctverec.
 */

// prace se vstupem/vystupem
#include <stdio.h>

// obecne funkce jazyka C
#include <stdlib.h>

// kvuli funkci strcmp
#include <string.h>

#include <ctype.h>
//int pocet =0;
/** Kody chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybny prikazovy radek. */
  ENOTOPEN,
  EINVALID,
  EMALLOC,
  EUNKNOWN,    /**< Neznama chyba */
};

/** Stavove kody programu */
enum tstates
{
  SHELP,
  STEST,
  SHLINE,
  SVLINE,
  SSQUARE,
};

/**Chybova hlaseni odpovidajici chybovym kodum. */
const char *ECODEMSG[] =
{
  [EOK] = "Vse v poradku.\n",
  [ECLWRONG] = "Chybne parametry prikazoveho radku!\n",
  [ENOTOPEN] = "Nepodarilo se otevrit soubor!\n",
  [EINVALID] = "Nebyl zadan validni obrazek!\n",
  [EMALLOC] = "Nepovedlo se alokovat prostor pro obrazek!\n",


  [EUNKNOWN] = "Nastala nepredvidana chyba! Vypnete pocitac a rychle utecte.\n",
};

// text napovedy
const char *HELPMSG =
  "Program Hledani obrazcu - proj3.\n"
  "Autor: Vojtech Sikula (c) 2013\n"
  "Program podle vstupnich parametru hleda v zadanem souboru\n"
  "        bud nejvetsi usecku, nebo ctverec."
  "Rezimy spusteni:\n"
  "         proj3 --help - vypise napovedu\n"
  "         proj3 --test s - zjisti jestli soubor s obsahuje validni obrazek\n"
  "         proj3 --hline s - najde nejvetsi horizontalni usecku\n"
  "         proj3 --vline s - najde nejvetsi vertikalni usecku\n"
  "         proj3 --square s - najde nejvetsi ctverec\n";


/**
 * Struktura pro ulozeni dvouromerneho pole
 */
typedef struct {
  int rows;
  int cols;
  char *cells;
} Bitmap;

/**
 * Struktura obsahujici hodnoty parametru prikazove radky.
 */
typedef struct params
{
  int ecode;         /**< Chybovy kod programu, odpovida vyctu tecodes. */
  int state;         /**< Stavovy kod programu, odpovida vyctu tstates. */

  char *s;
} TParams;

/**
 *  Deklarace f-ci
 */
char getcolor(Bitmap*, int, int);

int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2);
void printECode(int);
TParams getParams(int, char *[]);
int zkontroluj_a_preved(char *s, Bitmap *bitmap);
int square_online(Bitmap *bitmap, int x, int y, int vel,int *maximum, int *x1, int *x2, int *y1, int *y2);
int is_square(Bitmap *bitmap, int x, int y, int vel);

/////////////////////////////////////////////////////////////////
/**
 * Hlavni­ program.
 */
int main(int argc, char *argv[])
{
  int valid = 0;
  TParams params = getParams(argc, argv);
  if (params.ecode != EOK)
  { // neco nestandartniho
    printECode(params.ecode);
    return EXIT_FAILURE;
  }
// provadeni vypoctu podle zadanych parametru
  if (params.state == SHELP)
  {
    printf("%s", HELPMSG);
    return EXIT_SUCCESS;
  }
  if (params.state == STEST)
  {
    valid = zkontroluj_a_preved(params.s, NULL);
    if (valid == -1)
    {
      printECode(ENOTOPEN);
      return EXIT_FAILURE;
    }
    printf("%s\n",valid==1?"Valid":"Invalid");
  }
  else
  {
    int pom;
    int x1, x2, y1, y2;
    x1 = x2 = y1 = y2 = 0;
    Bitmap bitmap;
    pom = zkontroluj_a_preved(params.s, &bitmap);
    switch (pom)
    {
      case -2:
        printECode(EMALLOC);
        return EXIT_FAILURE;
        break;
      case -1:
        printECode(ENOTOPEN);
        return EXIT_FAILURE;
        break;
      case 0:
        printECode(EINVALID);
        return EXIT_FAILURE;
        break;
    }
    if (params.state == SHLINE)
    {
      pom = find_hline(&bitmap, &x1, &y1, &x2, &y2);
      if (pom != 0)
      {
        printf("%d %d %d %d\n",x1,y1,x2,y2);
      }
      else
      {
        printf("%d\n", pom);
      }
    }
    else if (params.state == SVLINE)
    {
      pom = find_vline(&bitmap, &x1, &y1, &x2, &y2);
      if (pom != 0)
      {
        printf("%d %d %d %d\n",x1,y1,x2,y2);
      }
      else
      {
        printf("%d\n", pom);
      }
    }
    else if (params.state == SSQUARE)
    {
      pom = find_square(&bitmap, &x1, &y1, &x2, &y2);

      if (pom != 0)
      {
        printf("%d %d %d %d\n",x1,y1,x2,y2);
      }
      else
      {
        printf("%d\n", pom);
      }
    }
  }
   // printf("%d\n", pocet);
  return EXIT_SUCCESS;
}
/**
 * funkce square_online
 * nalezeni ctverce na dane usecce vetsiho nez maximum
 * @param bitmap ukazatel na strukturu Bitmap.
 * @param x, y leveho horniho bodu ctverce.
 * @param vel velikost ctverce
 * @param maximum velikost nejvetsiho ctverce
 * @param x1, x2, y1, y2 souradnice nejvetsiho nalezeneho ctverce
 * @return Vraci nenulove cislo pokud se podarilo najit ctverec.
 */
int square_online(Bitmap *bitmap, int x, int y, int vel,int *maximum, int *x1, int *x2, int *y1, int *y2)
{
  int sqr = 0;
  for(int i = vel; i > *maximum; i--)
  {
    for (int j = 0; y+j+i <= y+vel; j++)
    {
      sqr = is_square(bitmap, x, y+j, i);
      if (sqr == 1)
      {
        *x1 = x;
        *y1 = y+j;
        *x2 = x + i-1;
        *y2 = *y1 + i-1;
        *maximum = i;
        return 1;
      }
    }
  }

  return 0;
}
/**
 * funkce is_square
 * zjisti jestli existuje dany ctverec
 * @param bitmap ukazatel na strukturu Bitmap.
 * @param x, y leveho horniho bodu ctverce.
 * @param vel velikost ctverce
 * @return Vraci nenulove cislo pokud se podarilo najit ctverec.
 */
int is_square(Bitmap *bitmap, int x, int y, int vel)
{

  for (int i = x; i<x+vel; i++)
  {
    if ((0x01 & getcolor(bitmap, i, y))==0)
      return 0;
  }
  for (int i = x; i<x+vel; i++)
  {
    if ((0x01 & getcolor(bitmap, i, y+vel-1))==0)
      return 0;
  }
  for (int i = y; i<y+vel; i++)
  {
    if ((0x01 & getcolor(bitmap, x+vel-1, i))==0)
      return 0;
  }
  return 1;
}
/**
 * funkce find_square
 * Najde nejdelsi horizontalni usecku
 * @param bitmap ukazatel na strukturu Bitmap.
 * @param x1, y1, x2, y2 souradnice nejdelsiho nalezeneho ctverce
 * @return Vraci nenulove cislo pokud se podarilo najit usecku.
 */
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
  int maximum = 0;
  int poc = 0;
  int px1 = 0;
  int py1 = 0;
  char xy;




  for (int i = 0; i < bitmap->rows; i++)
  {
    for (int j=0; j<bitmap->cols; j++)
    {
      xy = getcolor(bitmap, i, j);
      if ((
           (xy & 0x80) == 0) && (j == 0) )
        break;
      if ((0x01 & xy)== 1)
      {
        if (maximum == 0)
        {
          maximum = 1;
          *x1 = i;
          *x2 = i;
          *y1 = j;
          *y2 = j;
        }
        poc++;
      }
      else
      {
        if (poc > maximum)
        {
          square_online(bitmap, px1,py1,poc,&maximum,x1,x2, y1,y2);
        }
        poc = 0;
        px1 = i;
        py1 = j+1;
      }
    }
    if (poc > maximum)
    {
      square_online(bitmap, px1,py1,poc,&maximum,x1,x2, y1,y2);
    }
    px1 = i+1;
    py1 = 0;
    poc = 0;
  }
 // printf("maximum %d\n", maximum);
  if (maximum < 2)
  {
    return 0;
  }
  return 1;
}

/**
 * funkce find_vline
 * Najde nejdelsi horizontalni usecku
 * @param bitmap ukazatel na strukturu Bitmap.
 * @param x1, y1, x2, y2 souradnice nejdelsi nalezene usecky
 * @return Vraci nenulove cislo pokud se podarilo najit usecku.
 */
int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
  int maximum = 0;
  int poc = 0;
  int px1 = 0;
  int py1 = 0;
  char xy = 0;


  for (int i = 0; i < bitmap->cols; i++)
  {
    for (int j=0; j<bitmap->rows; j++)
    {
      xy = getcolor(bitmap, j, i);
      if ((xy & 0x40)== 0 && j == 0 )
        break;
      if ((0x01 & xy)== 1)
      {
        poc++;
      }
      else
      {
        if (poc >= maximum)
        {
          if ((px1 < *x1)&& (poc == maximum))
          {// pokud je stejne dlouha jako maximum a ma mensi index radku
            *x1 = px1;
            *y1 = py1;
            *x2 = j-1;
            *y2 = i;
            maximum = poc;
          }
          if (poc > maximum)
          {
            *x1 = px1;
            *y1 = py1;
            *x2 = j-1;
            *y2 = i;
            maximum = poc;
          }
        }
        poc = 0;
        px1 = j+1;
        py1 = i;
      }
    }
    if (poc > maximum)
    {
      maximum = poc;
      *x1 = px1;
      *y1 = py1;
      *x2 = bitmap->rows-1;
      *y2 = i;
    }
    px1 = 0;
    py1 = i+1;
    poc = 0;
  }
  if (maximum < 2)
  {
    return 0;
  }
  return 1;
}
/**
 * funkce find_hline
 * Najde nejdelsi horizontalni usecku
 * @param bitmap ukazatel na strukturu Bitmap.
 * @param x1, y1, x2, y2 souradnice nejdelsi nalezene usecky
 * @return Vraci nenulove cislo pokud se podarilo najit usecku.
 */
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
  int maximum = 0;
  int poc = 0;
  int px1 = 0;
  int py1 = 0;
  char xy;

  for (int i = 0; i < bitmap->rows; i++)
  {

    for (int j=0; j<bitmap->cols; j++)
    {
      xy = getcolor(bitmap, i,j);
      if ((xy & 0x80)== 0 && j == 0 )
        break;
      if ((0x01 & xy) == 1)
      {
        poc++;
      }
      else
      {
        if (poc > maximum)
        {
          *x1 = px1;
          *y1 = py1;
          *x2 = i;
          *y2 = j-1;
          maximum = poc;

        }
        poc = 0;
        px1 = i;
        py1 = j+1;
      }
    }
    if (poc > maximum)
    {
      maximum = poc;
      *x1 = px1;
      *y1 = py1;
      *x2 = i;
      *y2 = bitmap->cols-1;
    }
    px1 = i+1;
    py1 = 0;
    poc = 0;
  }
 // printf("maximum %d\n", maximum);
  if (maximum < 2)
  {
    return 0;
  }
  return 1;
}
/**
 * funkce zkontroluj_a_preved
 * @return   1   pokud soubor ma pozadovany format
 *          -2  pokud se nepovedlo alokovat misto pro matici
 *          -1  pokud se nepovedlo otevrit soubor,
 *          0   pokud soubor nema pozadovany format
 * @param *s jmeno souboru
 * @param bitmap ukazatel na strukturu Bitmap
 */

int zkontroluj_a_preved(char *s, Bitmap *bitmap)
{
  FILE *f;
  int r, c;
  int znak;
  char nenulovy_radek = 0; // pouze horni bit 1 (1000 0000)


  if ((f = fopen(s, "r")) == NULL)
  { // testovani otevreni souboru
    return -1;
  }
  if (fscanf(f, "%d %d",&r, &c) != 2)
  { // precteni prvnich dvou cisel rozmery bitmapy
    fclose(f);
    return 0;
  }
  if (bitmap != NULL)
  { // pokud byla funkce zavolana s platnym ukazatelem na strukturu
    bitmap->rows = r;
    bitmap->cols = c;
    if ((bitmap->cells = malloc(r*c*sizeof(char))) == NULL)
    { // alokace mista pro pole
      return -2;
    }
    // inicializace pole nulama
    for (int i = 0; i< r*c ; i++)
      bitmap->cells[i] = 0;
  }
  znak = fgetc(f);

  if(!(znak <= 0x20 || znak >= 0x7F))
  { // testovani bileho znaku
    fclose(f);
    return 0;
  }
  // nacitani dat do struktury Bitmap
  for (int i = 0; i < r; i++)
  {
    nenulovy_radek = 0;
    for (int j = 0; j < c; j++)
    {
      znak = fgetc(f);
      if (znak == '0' || znak == '1')
      {
        if (znak == '1' && bitmap != NULL)
        {
          nenulovy_radek = (char)0x80;
          bitmap->cells[j] = bitmap->cells[j] | 0x40;
        }
        if (bitmap != NULL)
        {
          bitmap->cells[i*c + j] = ((char) (znak - '0'));
          if (j == c-1)
            bitmap->cells[i*c] = bitmap->cells[i*c] | nenulovy_radek;
        }
      }
      else
      {

        if (bitmap != NULL)
          free(bitmap->cells);
        return 0;
      }
      znak = fgetc(f);
      if((!(znak <= 0x20 || znak >= 0x7F)))
      {
        fclose(f);
        return 0;
      }
      //printf("%d%s",getcolor(bitmap,i,j), j==c-1?"\n":"");
    }
  }


  fclose(f);
  return 1;

}

/**
 * funkce getcolor
 * vraci informace o barve na zadanych souradnicich
 * @param *bitmap ukazatel na strukturu bitmap
 * @param x x-ova souradnice
 * @param y y-ova souradnice
 */
char getcolor(Bitmap *bitmap, int x, int y)
{
//  pocet++;
  return (bitmap->cells[bitmap->cols*x + y]);
}
/**
 * funkce printECode
 * Vytiskne hlaseni odpovidajici chybovemu kodu.
 * @param ecode kod chyby programu
 */
void printECode(int ecode)
{
  if (ecode < EOK || ecode > EUNKNOWN)
  { ecode = EUNKNOWN; }
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}

/**
 * funkce getParams
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams.
 * Pokud je format argumentu chybny, ukonci program s chybovym kodem.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Vraci analyzovanĂŠ argumenty prikazoveho radku.
 */
TParams getParams(int argc, char *argv[])
{
  TParams result =
  { // inicializace struktury
    .ecode = EOK,
    .state = SHELP,
    .s = NULL,
  };

  if(argc == 2)
  {
    if(strcmp("--help", argv[1]) == 0)
    {
      result.state = SHELP;
    }
    else
    {
          result.ecode = ECLWRONG;
    }
  }
  if (argc == 3)
  {
    result.s = argv[2];
    if (strcmp("--test", argv[1])== 0)
    {
      result.state = STEST;
    }
    else if (strcmp("--hline", argv[1])== 0)
    {
      result.state = SHLINE;
    }
    else if (strcmp("--vline", argv[1]) == 0)
    {
      result.state = SVLINE;
    }
    else if (strcmp("--square", argv[1]) == 0)
    {
      result.state = SSQUARE;
    }
    else
    {
      result.ecode = ECLWRONG;
    }
  }
  if (argc < 2 &&  argc < 3)
  { // nespravny pocet argumentu

    result.ecode = ECLWRONG;
  }
  return result;
}

/* konec proj3.c */
