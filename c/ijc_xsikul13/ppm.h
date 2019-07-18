  // ppm.h
  // �e�en� IJC-DU1,  23.3.2014
  // Autor: Vojt�ch �ikula, FIT
  // P�elo�eno: GNU gcc compiler
  // popis p��kladu - pozn�mky, atd
#ifndef PPM_H_INCLUDED
#define PPM_H_INCLUDED


     struct ppm {
        unsigned xsize;
        unsigned ysize;
        char data[];    // RGB bajty, celkem 3*xsize*ysize
     };

    struct ppm * ppm_read(const char * filename);


  //  int ppm_write(struct ppm *p, const char * filename);


#endif // PPM_H_INCLUDED
