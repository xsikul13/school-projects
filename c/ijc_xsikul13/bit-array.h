  // bit-array.h
  // Øešení IJC-DU1,  23.3.2014
  // Autor: Vojtìch Šikula, FIT
  // Pøeloženo: GNU gcc compiler
  // Vypada to strasne



#ifndef BIT_ARRAY_H_INCLUDED
#define BIT_ARRAY_H_INCLUDED
#include "error.h"

#define BitArray(jmeno_pole, velikost) unsigned long jmeno_pole[] = {[0]=(velikost), [PocetPrvku(velikost)]= 0}
#define PocetBituU (sizeof(unsigned long)*8)
#define PocetPrvku(pocet) (pocet)%(PocetBituU)?IndexPrvku(pocet)+1 : IndexPrvku(pocet)

// pomocna makra
#define IndexPrvku(index) (((index)/(PocetBituU))+1)
#define IndexBitu(index) ((index)%(PocetBituU))
#define Maska(index) ((~((unsigned long)~0<<1))<<(index))


typedef unsigned long BitArray_t[];



#ifndef USE_INLINE // -DUSE_INLINE

//vrati velikost bitoveho pole (pocet bitu) -> sizeof(pole) ve funkci vraci­ velikost adresy (ala pascal)
#define BitArraySize(jmeno_pole) (pole[0])
// pomocna makra diki za ne! :)
#define DU1__SET_BIT(p,i,b) ((b==0)?(  p[IndexPrvku(i)] &= ~(Maska(IndexBitu(i)))) : ( p[IndexPrvku(i)] |= Maska(IndexBitu(i))))
#define DU1__GET_BIT(p,i) ((p[IndexPrvku(i)] & Maska(IndexBitu(i)))?1:0)
// rozÅ¡Ã­Å™enÃ­ o hlÃ­dÃ¡nÃ­ mezÃ­


#define SetBit(jmeno_pole, index, vyraz) \
    (BitArraySize(jmeno_pole) >= (index))? (DU1__SET_BIT(jmeno_pole, (index), (vyraz))) : (FatalError("SIndex %ld mimo rozsah 0..%ld", ((long)index, (long)jmeno_pole[0])))
//#define SetBit(array, index, value) (index) >= array[0] ? FatalError("Index %lu mimo rozsah 0..%lu", (index), array[0]-1),0 \
                                                        : (DU1__SET_BIT(array, (index), (value)))
#define GetBit(jmeno_pole, index) \
     ((BitArraySize(jmeno_pole) >= (index)) ? DU1__GET_BIT(jmeno_pole, (index)) : (FatalError("GIndex %ld mimo rozsah 0..%ld", (long)index, (long)jmeno_pole[0]),0))
//#define GetBit(array, index) (index) >= array[0] ? FatalError("Index %lu mimo rozsah 0..%lu", (index), array[0]-1),0 \
                                                 : (DU1__GET_BIT(array, (index)))


// inline f-ce
#else
//inline unsigned long BitArraySize(BitArray_t );
//inline void SetBit(BitArray_t , unsigned long , int );
//inline int GetBit(BitArray_t , unsigned long );


//inline unsigned long BitArraySize(BitArray_t jmeno_pole){return jmeno_pole[0];}

inline void SetBit(BitArray_t jmeno_pole, unsigned long index, int vyraz)
{
	//printf("inline \t");
    if (jmeno_pole[0] < index){
        FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)jmeno_pole[0]);
        return;
    }

    if (vyraz == 0){
		            jmeno_pole[(index/(sizeof(jmeno_pole[0])*8)+1+(index%(sizeof(jmeno_pole[0])*8)?0:1))] &=  ( ~((~(~(unsigned long)0<<1))<<(index%(sizeof(jmeno_pole[0])*8))) );
     //           jmeno_pole[index/(sizeof(jmeno_pole[0])*8) + 1 + (index%(sizeof(jmeno_pole[0])*8)?0:1)] &=   ~(1<<(index%(sizeof(jmeno_pole[0])*8)));
            return;}
    jmeno_pole[(index/(sizeof(jmeno_pole[0])*8)+1+(index%(sizeof(jmeno_pole[0])*8)?0:1))] |= \
		(unsigned long)(~(~((unsigned long)0)<<1)<<(index%(sizeof(jmeno_pole[0])*8)));
    return ;								
}

inline int GetBit(BitArray_t jmeno_pole, unsigned long index)
{
	//printf("inline \t");
    if (jmeno_pole[0] < index)
    {
        FatalError("Index %ld mimo rozsah 0..%ld", (long)index, (long)jmeno_pole[0]);
        return -1;
    }
    return jmeno_pole[(index/(sizeof(jmeno_pole[0])*8))+1+(index%(sizeof(jmeno_pole[0])*8)?0:1)] &  ((~(~(unsigned long)0<<1))<<(index%(sizeof(jmeno_pole[0])*8)) ) ? 1:0;
}




#endif // USE_INLINE


#endif
