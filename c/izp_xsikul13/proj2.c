/*
 * Soubor:  proj2.c
 * Datum:   2013/11/25
 * Autor:   Vojtech Sikula, xsikul13@stud.fit.vutbr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program podle vstupnich parametru pocita
 *          bud odmocninu, arkus sinus nebo uhly trojuheliku.
 */

// prace se vstupem/vystupem
#include <stdio.h>

// obecne funkce jazyka C
#include <stdlib.h>

// kvuli funkci strcmp
#include <string.h>

#define EPS 0.000000000001
#define PI_PUL 1.57079632679
#define NAN (0.0/0.0)

/** Kody chyb programu */
enum tecodes
{
  EOK = 0,     /**< Bez chyby */
  ECLWRONG,    /**< Chybný prikazovy radek. */
  EUNKNOWN,    /**< Neznama chyba */
};

/** Stavové kody programu */
enum tstates
{
  SHELP,         /**< Napoveda */
  SSQRT,      /**< Pocitani odmocniny */
  SASIN,         /**< Pocitani arcus sinus */
  STRIANGLE,
};

/** Chybová hlášení odpovídající chybovým kódùm. */
const char *ECODEMSG[] =
{
  [EOK] = "Vse v poradku.\n",
  [ECLWRONG] = "Chybne parametry prikazoveho radku!\n",

  [EUNKNOWN] = "Nastala nepredvidana chyba! Vypnete pocitac a rychle utecte.\n",
};

// text napovedy
const char *HELPMSG =
  "Program Iteracni vypocty - proj2.\n"
  "Autor: Vojtech Sikula (c) 2013\n"
  "Program podle vstupnich parametru pocita\n"
  "        bud odmocninu, arkus sinus nebo uhly trojuheliku."
  "Rezimy spusteni:\n"
  "         proj2 --help - vypise napovedu\n"
  "         proj2 --asin X - vypocita arkus sinus X\n"
  "         proj2 --sqrt X - vypocita odmocninu X\n"
  "         proj2 --triangle AX AY BX BY CX CY - vypocita uhly\n"
  "                         trojuhelniku podle zadanych souradnic\n";


/**
 * souradnice jednotlivych polozkach v typu TTriangle
 */
enum tpoints
{
    A = 0,
    B,
    C,
};
enum tsour
{
    X = 0,
    Y,
};
 /**
  * Typ pro ulozeni souradnic trojuhelniku
  */
typedef double TPoints[3][2]; // tri body se souradnicemi x a y
/**
  * Typ pro ulozeni uhlu trojuhelniku
  */
typedef double TUhly[3];
/**
  * Struktura obsahujici body trojuhelniku a jeho uhly
  */
typedef struct
{
  TPoints points;
  TUhly uhly;
}TTriangle;

/**
 * Struktura obsahujici hodnoty parametru prikazove radky.
 */
typedef struct params
{
  unsigned short N;  /**< Hodnota N z prikazove radky. */
  int ecode;         /**< Chybovy kod programu, odpovida vyctu tecodes. */
  int state;         /**< Stavovy kod programu, odpovida vyctu tstates. */

  double  Xsqrt;
  double Xasin;
  TTriangle triangle;
} TParams;

/**
 *  Deklarace f-ci
 */
double my_sqrt(double);
double my_asin(double);
void uhly(TTriangle *);
void printECode(int);
TParams getParams(int, char *[]);
double my_fabs(double);
/////////////////////////////////////////////////////////////////
/**
 * Hlavní program.
 */
int main(int argc, char *argv[])
{
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
  else if (params.state == SSQRT)
  {
    printf("%.10e\n",my_sqrt(params.Xsqrt));
  }
  else if (params.state == SASIN)
  {
    printf("%.10e\n",my_asin(params.Xasin));
  }
  else if (params.state == STRIANGLE)
  {
    uhly(&params.triangle);
    for (int i = 0; i<3; i++)
    {
      printf("%.10e\n",params.triangle.uhly[i]);
    }
  }
  return EXIT_SUCCESS;
}

/**
 * Funkce my_sqrt
 * @param x argument funkce odmocnina
 * @return vraci odmocninu x
 */
double my_sqrt(double x)
{
  double yp = 0;
  double yd = 1;
  if (x == 0)
    return 0;
  if (x < 0)
    return NAN;
  while (my_fabs(yp-yd) >= yd*EPS)
  {
    yp = yd;
    yd = (x/yp + yp)*0.5;
  }
  return yd;
}

/**
 * Funkce my_asin
 * @param argument funkce Arcus sinus
 * @return vraci vysledek Arcus sinus
 */
double my_asin(double x)
{
    double t = 1;
    double yd = 0;
    double yp = 0;
    double n = 1;
    double xnai = x;
    double xna2 = x*x;
    unsigned long long i = 1;

  if (x > 1 || x < -1)
  {
    return NAN;
  }
  if (x == 0)
  {
    return 0;
  }
  else if ((1-x)< EPS)
  {
    return PI_PUL;
  }
  else if ((x+1)< EPS)
  {
    return -PI_PUL;
  }
  while(my_fabs(t) >= my_fabs(yd)*EPS)
  {
    yp = yd;
    t = n*xnai/i;
    yd = yp + t;
    i+=2;
    xnai *= xna2;
    n *= ((double)(i-2))/(i-1);
  }
  return yd;
}

/**
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
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams.
 * Pokud je format argumentu chybny, ukonci program s chybovym kodem.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Vraci analyzované argumenty prikazoveho radku.
 */
TParams getParams(int argc, char *argv[])
{
  TParams result =
  { // inicializace struktury
    .N = 0,
    .ecode = EOK,
    .state = SSQRT,
    .Xsqrt = 0,
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
    char *chyba;
    if(strcmp("--sqrt", argv[1]) == 0)
    {
      result.Xsqrt = strtod(argv[2], &chyba);
      if (*chyba == '\0')
      {
        result.state = SSQRT;
      }
      else
      {
        result.ecode = ECLWRONG;
      }
    }
    else if(strcmp("--asin", argv[1]) == 0)
    {
      result.Xasin = strtod(argv[2], &chyba);
      if (*chyba == '\0')
      {
        result.state = SASIN;
      }
      else
      {
        result.ecode = ECLWRONG;
      }
    }
    else
    {
      result.ecode = ECLWRONG;
    }
  }
  if (argc == 8)
  {
    result.state = STRIANGLE;
    if(strcmp("--triangle", argv[1]) == 0)
    {
      char *chyba;
      int i = 2;
      for (int m = 0; m <= C; m++)
      {
        for (int n = 0; n <= Y; n++)
        {
          result.triangle.points[m][n] = strtod(argv[i++], &chyba);
          if (*chyba != '\0')
          {
            result.ecode = ECLWRONG;
          }
        }
      }
    }
  }
  if (argc != 2 && argc != 3 && argc != 8)
  { // nespravny pocet argumentu
    result.ecode = ECLWRONG;
  }
  return result;
}

/**
 * Funkce uhly
 * @param t struktura TTriangle
 * @return void
 */
void uhly(TTriangle *t)
{
  double a, b, c;
  double kc; // konstanta c u normalove rovnice primky ax + bx + c == 0

  kc = -(t->points[A][X] * (-(t->points[A][Y] - t->points[B][Y]))+t->points[A][Y] * (t->points[A][X] - t->points[B][X]));
  if ((-(t->points[A][Y]-t->points[B][Y])) * t->points[C][X]+(t->points[A][X]-t->points[B][X]) * t->points[C][Y] + kc == 0)
  {
    // zjisteni jestli lezi vsechny body na stejne primce, tzn. nejedna se o trojuhelnik
    t->uhly[0] = NAN;
    t->uhly[1] = NAN;
    t->uhly[2] = NAN;
    return;
  }
  // vypocet delek stran
  a = ((t->points[B][X]-t->points[C][X])*(t->points[B][X]-t->points[C][X])) + ((t->points[B][Y]-t->points[C][Y])*(t->points[B][Y]-t->points[C][Y]));
  b = (t->points[A][X]-t->points[C][X])*(t->points[A][X]-t->points[C][X]) + (t->points[A][Y]-t->points[C][Y])*(t->points[A][Y]-t->points[C][Y]);
  c = (t->points[B][X]-t->points[A][X])*(t->points[B][X]-t->points[A][X]) + (t->points[B][Y]-t->points[A][Y])*(t->points[B][Y]-t->points[A][Y]);
  a = my_sqrt(a);
  b = my_sqrt(b);
  c = my_sqrt(c);
  // vypocet uhlu trojuhelniku
  t->uhly[0] = my_asin(-((b*b+c*c-a*a)/(2*b*c)))+ PI_PUL;
  t->uhly[1] = my_asin(-((a*a+c*c-b*b)/(2*a*c)))+ PI_PUL;
  t->uhly[2] = my_asin(-((a*a+b*b-c*c)/(2*a*b)))+ PI_PUL;

  return;
}

/**
 * Funkce my_fabs
 * @param x cislo typu double
 * @return vraci absolutni hodnotu x
 */
double my_fabs(double x)
{
  if (x<0)
  {
    return -x;
  }
  else
  {
    return x;
  }
}
/* konec proj2.c */
