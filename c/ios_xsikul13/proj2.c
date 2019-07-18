// Autor: Vojtěch Šikula (xsikul13)
// projek pro předmět IOS - ls 2014
// zadádí: vis stránky předmětu
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
 
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sched.h>

// kod chyby 
#define E_SHR 1
#define E_FORK 2
#define E_PAR 3
#define E_FILE 4
#define E_SEM 5
#define E_SHM 6

 
// pro zjednodušení globální proměnné (možnost předávat ve struktuře, ale ...)
 
int P, H, S, R; // parametry spuštění
int A_shm, *A; // počítadlo výpisů
int Hmolo_shm, *Hmolo, Smolo_shm, *Smolo; //počítadlo serfů a heckerů na molu
int Hstart_shm, *Hstart, Sstart_shm, *Sstart; // počítadlo spuštěných S H
//int Hdone_shm, *Hdone, Sdone_shm, *Sdone; //počítadlo serfů a heckerů done
int Nalodi_shm, *Nalodi; // počet lidí na lodi
FILE *f; 
sem_t *SEM_pamet, *SEM_molo, *SEM_SQ, *SEM_HQ, *SEM_konec, *SEM_member, *SEM_plavba, *SEM_zavora; 
pid_t pidS, pidH; // pid procesů generující serf(y|i) hacker(y|i)

// funkční prototypy
void serf_gen(int);
void hacker_gen(int);
void serf();
void hacker();
void zpracuj_param(int argc, char *argv[]);
void chyba(int cislo);
void uvolni_zdroje();
void vytvor_shm();
void vytvor_sem();
void napoveda();
void uklid(); 

  ////////////////////////////////////////////////////////////
 /*********************** MAIN *****************************/
//////////////////////////////////////////////////////////// 
int main(int argc, char *argv[])
{
  signal(SIGINT, uklid);
  signal(SIGKILL, uklid);

  zpracuj_param(argc,argv);
  f=fopen("rivercrossing.out","w");
  if (f==NULL){chyba(E_FILE);}
    setbuf(f, NULL);


  vytvor_shm(); //vytvoreni sdilené paměti
  vytvor_sem(); //vytvoreni semaforů

  pidS=fork(); //generátor serfů
  if (pidS==0){
    serf_gen(P);
  }else if(pidS<0){ 
    uvolni_zdroje();
    chyba(E_FORK); }
  
  pidH=fork(); //generátor hackerů
  if (pidH==0){
    hacker_gen(P);
  }else if (pidS<0) {
    kill(pidS, SIGKILL); // ještě smazat vytvorené SERFi
    uvolni_zdroje();

    chyba(E_FORK); }
    
  waitpid(pidS, NULL, 0); //čekáme na ukončení serf_gen
  waitpid(pidH, NULL, 0); //čekáme na ukončení hacker_gen 
 

  fclose(f); 
  uvolni_zdroje();
 
  return EXIT_SUCCESS;
}
  ////////////////////////////////////////////////////////////
 //                       MAIN - konec                     //
//////////////////////////////////////////////////////////// 

// vytvor sdilenou pamet
void vytvor_shm(){

//zde se inkrementuje číslo tisknutelné operace
  if ((A_shm=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666))==-1){
    chyba(E_SHR);
  }
  if((A=(int*)shmat(A_shm, NULL, 0)) == (void *) -1){
    chyba(E_SHM);
  }
  *A=0; // sdílená paměť se automaticky nuluje, ale co... :))
 
// počet serfů na molu
  if((Smolo_shm=shmget(IPC_PRIVATE, sizeof(int),IPC_CREAT | 0666))==-1){
    chyba(E_SHR);
  }
  if((Smolo=(int*)shmat(Smolo_shm, NULL, 0)) == (void *) -1){
    uvolni_zdroje(); chyba(E_SHM);
  }
  *Smolo=0;

//jednoznačný identifikátor kaľdého zákazníka
  if((Hmolo_shm=shmget(IPC_PRIVATE, sizeof(int),IPC_CREAT | 0666))==-1){
    chyba(E_SHM);
  }
  if((Hmolo=(int*)shmat(Hmolo_shm, NULL, 0)) == (void *) -1){
    uvolni_zdroje(); chyba(E_SHM);
  }
  *Hmolo=0;

//proměnná UP_and_DOWN:) 0->4 4->0, abychom se vyhnuly aktivnímu čekání
  if((Nalodi_shm=shmget(IPC_PRIVATE, sizeof(int),IPC_CREAT | 0666))==-1){
    chyba(E_SHM);
  }
  if((Nalodi=(int*)shmat(Nalodi_shm, NULL, 0)) == (void *) -1){
    uvolni_zdroje(); chyba(E_SHM);
  }
  *Nalodi=0;

//jednoznačný identifikátor serfů
  if((Sstart_shm=shmget(IPC_PRIVATE, sizeof(int),IPC_CREAT | 0666))==-1){
    chyba(E_SHM);
  }
  if((Sstart=(int*)shmat(Sstart_shm, NULL, 0)) == (void *) -1){
    uvolni_zdroje(); chyba(E_SHM);
  }
  *Sstart=0;

//jednoznačný identifikátor hackerů
  if((Hstart_shm=shmget(IPC_PRIVATE, sizeof(int),IPC_CREAT | 0666))==-1){
    chyba(E_SHM);
  }
  if((Hstart=(int*)shmat(Hstart_shm, NULL, 0)) == (void *) -1){
    uvolni_zdroje(); chyba(E_SHM);
  }
  *Hstart=0;
} 


// vytvoreni semaforů
void vytvor_sem(){
  SEM_pamet = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_pamet, 1, 1))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }//vstup do sdílené paměti (OTEVŘENO)

  SEM_molo = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_molo, 1, 1))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }// vstup na molo (OTEVŘENO)

  SEM_SQ = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_SQ, 1, 0))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }// čekání serfů na nalodění (ZAVŘENO)

  SEM_HQ = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_HQ, 1, 0))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }// čekání hackerů na nalodění (ZAVŘENO)

  SEM_konec = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_konec, 1, 0))==-1){
    uvolni_zdroje();;  chyba(E_SEM);
  }// čekání až skončí všechny plavby (ZAVŘENO)

  SEM_member = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_member, 1, 0))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }//čekání na posádku (ZAVŘENO)

  SEM_plavba = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_plavba, 1, 0))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }//(ZAVŘENO)

SEM_zavora = (sem_t *)mmap(0, sizeof(sem_t),
           PROT_READ | PROT_WRITE,MAP_ANON | MAP_SHARED, -1, 0);
  if((sem_init(SEM_zavora, 1, 0))==-1){
    uvolni_zdroje();  chyba(E_SEM);
  }//(ZAVŘENO)
}  

// generátor serfů
void serf_gen(int P){
  pid_t pid, pid_serf[P];
  for (int i = 0; i < P; i++) pid_serf[P] = 0;

  for (int i = 0; i < P; i++)
  {
    usleep(rand()%(S+1)*1000);
    pid = fork();
    if (pid == 0) { serf();
    }else if (pid < 0){
      //kill(p, SIGKILL);
      for (int j=0; j<i;i++){kill(pid_serf[j], SIGKILL);}
      uvolni_zdroje();
      chyba(E_FORK);
    }else if (pid>0){
      pid_serf[i]=pid;
    }
  } 
	
  
  for (int i = 0; i< P ; i++)
  	waitpid(pid_serf[i], NULL, 0); //čekáme na ukončení serfů 

  shmdt(A);
  shmdt(Smolo);
  shmdt(Hmolo);
  shmdt(Nalodi);
  shmdt(Sstart);
  shmdt(Hstart);


  exit(EXIT_SUCCESS);
}
// průběh serfa
void serf(){
  int captain = 0;
  int I;

  sem_wait(SEM_pamet);
    (*A)++; I = ++(*Sstart);
    fprintf(f,"%d: serf: %d: started\n", *A, I);
  sem_post(SEM_pamet);

  sem_wait(SEM_molo);
    (*Smolo)++;
    sem_wait(SEM_pamet);
      (*A)++; fprintf(f,"%d: serf: %d: waiting for boarding: %d: %d\n", *A, I, *Hmolo, *Smolo);
    sem_post(SEM_pamet);
    if (*Smolo == 4){ //podle typu vypravy kapitan povoli nalodeni
      *Smolo -= 4; captain = 1; 
      sem_post(SEM_SQ);sem_post(SEM_SQ);sem_post(SEM_SQ);sem_post(SEM_SQ);
    }else if (*Smolo == 2 && *Hmolo >=2){
      captain = 1; *Smolo -= 2; *Hmolo -= 2;
      sem_post(SEM_SQ);sem_post(SEM_SQ);sem_post(SEM_HQ);sem_post(SEM_HQ);          
    } else { sem_post(SEM_molo);   }
  
  sem_wait(SEM_SQ); 
    sem_wait(SEM_pamet);
      (*A)++; (*Nalodi)++;
      if (*Nalodi == 4 ) 
	{sem_post(SEM_member);sem_post(SEM_member);sem_post(SEM_member);sem_post(SEM_member);}
      fprintf(f,"%d: serf: %d: boarding: %d: %d\n", *A, I, *Hmolo, *Smolo);
    sem_post(SEM_pamet);
    
    sem_wait(SEM_member);
      sem_wait(SEM_pamet);
        (*A)++; (*Nalodi)--;
        fprintf(f,"%d: serf: %d: %s\n", *A, I, (captain?"captain":"member"));
        if (*Nalodi == 0) {sem_post(SEM_plavba);}
      sem_post(SEM_pamet);
      if (captain == 1) { 
        sem_wait(SEM_plavba); 
          //usleep(rand()%(R+1)*1000);
          sem_post(SEM_zavora);sem_post(SEM_zavora);sem_post(SEM_zavora);sem_post(SEM_zavora);
      }

 
      sem_wait(SEM_zavora);
        sem_wait(SEM_pamet);
          
          (*A)++; fprintf(f,"%d: serf: %d: landing: %d: %d\n", *A, I, *Hmolo, *Smolo);
          (*Nalodi)++; if ((*Nalodi) == 4) {sem_post(SEM_molo); *Nalodi = 0;}
        sem_post(SEM_pamet);
      
      sem_wait(SEM_pamet);
        if (*A >= 5*2*P) { sem_post(SEM_konec);}
      sem_post(SEM_pamet);
      sem_wait(SEM_konec);
        sem_wait(SEM_pamet);
          (*A)++;fprintf(f,"%d: serf: %d: finished\n", *A, I); 
        sem_post(SEM_pamet);
      sem_post(SEM_konec);
   
       

  shmdt(A);
  shmdt(Smolo);
  shmdt(Hmolo);
  shmdt(Nalodi);
  shmdt(Sstart);
  shmdt(Hstart);

  exit(EXIT_SUCCESS);

}

// funkce generujici hackery
void hacker_gen(int P){

  pid_t pid, pid_hacker[P];
  for (int i = 0; i < P; i++) pid_hacker[P] = 0;

  for (int i = 0; i < P; i++)
  {
    usleep(rand()%(H+1)*1000);
    pid = fork();
    if (pid == 0) { hacker();
    }else if (pid < 0){
      kill(pidS, SIGKILL);
      for (int j=0; j<i;i++){kill(pid_hacker[j], SIGKILL);}
      uvolni_zdroje();
      chyba(E_FORK);
    }else if (pid>0){
      pid_hacker[i]=pid;
    }
  } 
for (int i = 0; i < P; i++)		
  waitpid(pid_hacker[i], NULL, 0); //čekáme na ukončení hackerů 

  shmdt(A);
  shmdt(Smolo);
  shmdt(Hmolo);
  shmdt(Nalodi);
  shmdt(Sstart);
  shmdt(Hstart);

  exit(EXIT_SUCCESS);
}
 
void hacker(){
  int captain = 0;
  int I;

  sem_wait(SEM_pamet);
    (*A)++; I = ++(*Hstart);
    fprintf(f,"%d: hacker: %d: started\n", *A, I);
  sem_post(SEM_pamet);

  sem_wait(SEM_molo);
    (*Hmolo)++;
    sem_wait(SEM_pamet);
      (*A)++; fprintf(f,"%d: hacker: %d: waiting for boarding: %d: %d\n", *A, I, *Hmolo, *Smolo);
    sem_post(SEM_pamet);
    if (*Hmolo == 4){
      *Hmolo -= 4; captain = 1; 
      sem_post(SEM_HQ);sem_post(SEM_HQ);sem_post(SEM_HQ);sem_post(SEM_HQ);
    }else if (*Hmolo == 2 && *Smolo >=2){
      captain = 1; *Smolo -= 2; *Hmolo -= 2;
      sem_post(SEM_SQ);sem_post(SEM_SQ);sem_post(SEM_HQ);sem_post(SEM_HQ);          
    } else { sem_post(SEM_molo);   }
   
  sem_wait(SEM_HQ); 
    sem_wait(SEM_pamet);
      (*A)++; (*Nalodi)++;
      if (*Nalodi == 4 ) 
	{sem_post(SEM_member);sem_post(SEM_member);sem_post(SEM_member);sem_post(SEM_member);}
      fprintf(f,"%d: hacker: %d: boarding: %d: %d\n", *A, I, *Hmolo, *Smolo);
    sem_post(SEM_pamet);
    
    sem_wait(SEM_member);
      sem_wait(SEM_pamet);
        (*A)++; (*Nalodi)--;
        fprintf(f,"%d: hacker: %d: %s\n", *A, I, (captain?"captain":"member"));
        if (*Nalodi == 0) {sem_post(SEM_plavba);}
      sem_post(SEM_pamet);
      if (captain == 1) { 
        sem_wait(SEM_plavba);
          usleep(rand()%(R+1)*1000);
          sem_post(SEM_zavora);sem_post(SEM_zavora);sem_post(SEM_zavora);sem_post(SEM_zavora);
      }

      sem_wait(SEM_zavora);
        sem_wait(SEM_pamet);
          (*A)++; fprintf(f,"%d: hacker: %d: landing: %d: %d\n", *A, I, *Hmolo, *Smolo);
	  (*Nalodi)++; if ((*Nalodi) == 4) {sem_post(SEM_molo); *Nalodi = 0;}
        sem_post(SEM_pamet);
      
      
      sem_wait(SEM_pamet);
        if (*A >= 5*2*P) { sem_post(SEM_konec);}
      sem_post(SEM_pamet);
      sem_wait(SEM_konec);
        sem_wait(SEM_pamet);
          (*A)++;fprintf(f,"%d: hacker: %d: finished\n", *A, I); 
        sem_post(SEM_pamet);
      sem_post(SEM_konec); 
  


  shmdt(A);
  shmdt(Smolo);
  shmdt(Hmolo);
  shmdt(Nalodi);
  shmdt(Sstart);
  shmdt(Hstart);

  exit(EXIT_SUCCESS);

}
 
// spracovani prametru
void zpracuj_param(int argc, char *argv[]){

  if ((argc==2) && (strcmp("-h", argv[1])==0)){
      napoveda();exit(EXIT_SUCCESS);
  } //tisk nápovědy

  
  if (argc!=5){
    chyba(E_PAR);
  }else{
 
    if(isdigit(*argv[1])){
      P=atoi(argv[1]);
      if (P <= 0 || P % 2 != 0){chyba(E_PAR);}
    }else{
      chyba(E_PAR);
    }
    if(isdigit(*argv[2])){
      H =atoi(argv[2]);
      if ( H < 0 || H > 5001){chyba(E_PAR);}
    }else{
      chyba(E_PAR);
    }
    if(isdigit(*argv[3])){
      S = atoi(argv[3]);
      if (S < 0 || S > 5001){chyba(E_PAR);}
    }else{
      chyba(E_PAR);
    }
    if(isdigit(*argv[4])){
      R = atoi(argv[4]);
      if (R < 0 || R > 5001){chyba(E_PAR);}
    }else{
      chyba(E_PAR);
    }
  }
  
}

// pri zachyceni sygnalu uvolni zdroje a ukonci program 
void uklid(){
  uvolni_zdroje();
  exit(EXIT_FAILURE);
}
 
// uvolneni sdilene pameti a semaforu
void uvolni_zdroje(){ 
 int error = -1; 
  if (sem_destroy(&(*SEM_pamet))==-1){error = E_SHR ;}
  if (sem_destroy(&(*SEM_molo))==-1){error = E_SHR ;}
  if (sem_destroy(&(*SEM_SQ))==-1){error = E_SHR ;}
  if (sem_destroy(&(*SEM_HQ))==-1){error = E_SHR ;}
  if (sem_destroy(&(*SEM_member))==-1){error = E_SHR ;}
  if (sem_destroy(&(*SEM_plavba))==-1){error = E_SHR ;}
  if (sem_destroy(&(*SEM_konec))==-1){error = E_SHR ;} 
  if(shmctl(A_shm, IPC_RMID, NULL) == -1) { error = E_SHR ; }  
  if(shmctl(Smolo_shm, IPC_RMID, NULL) == -1) { error = E_SHR ; }
  if(shmctl(Hmolo_shm, IPC_RMID, NULL) == -1) { error = E_SHR ; }
  if(shmctl(Nalodi_shm, IPC_RMID, NULL) == -1) { error = E_SHR ; }
  if(shmctl(Sstart_shm, IPC_RMID, NULL) == -1) { error = E_SHR ; }
  if(shmctl(Hstart_shm, IPC_RMID, NULL) == -1) { error = E_SHR ; }

  if (error == E_SHR) chyba(E_SHR );
}

// chybové hlaseni a ukonceni programu
void chyba(int cislo){
  switch (cislo){
  case E_SHM: fprintf(stderr, "CHYBA - vytvareni sdilene pameti.\n");
          break;
  case E_FORK: fprintf(stderr, "CHYBA - vytvareni procesu.\n");
          break;
  case E_PAR: fprintf(stderr, "CHYBA - chybne parametry.\n");
          break;
  case E_FILE: fprintf(stderr, "CHYBA - Nepodarilo se spravne otevrit soubor pro zapis.\n");
          break;
  case E_SEM: fprintf(stderr, "CHYBA - nepodarilo se spravne vytvorit semafor.\n");
          break;
  case E_SHR: fprintf(stderr, "CHYBA - uvolneni sdilenych prostredku.\n");
          break;

  default: fprintf(stderr, "CHYBA - chyba programu. \n");
  }

  exit(EXIT_FAILURE);
}

// tisk napovedy
void napoveda(){
  printf("Autor: Vojtech Sikula (xsikul13)\n"
         "Program: rivercrossing\n"
         "synchronizacniho problemu spiciho holice pomoci semaforu\n"
         "./riverrcossing P H S R -\n"
         "parametry:                \n"
	 "   - P - pocet hackeuů a serfu (P>0 && Pmod2==0)\n"
	 "   - H - maximalni spozdeni generovani hackeru (v milisekundach (H>=0 && H<5001))\n" 
 	 "   - S - maximalni spozdeni generovani serfu (v milisekundach (S>=0 && S<5001))\n" 
 	 "   - R - maximalni doba plavby (v milisekundach (R>=0 && R<5001))\n"
         "vice info na (IOS 2013/14 - 2. projekt\n"  );
}