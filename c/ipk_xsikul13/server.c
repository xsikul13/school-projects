#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


struct Zaznam {
   char login[20];
   char heslo[20];
   char uid[10];
   char gid[10];
   char popis[255];
   char home[255];
   char shell[255];
};

int nacti(FILE * f, char *z );
void obsluha_clienta (int sock);
int get_zaznam(FILE *, struct Zaznam *);
void co_chces(char *buffer, char *zprava, struct Zaznam zaznam);

int main( int argc, char *argv[] )
{
  
    if (argc != 3){
      exit(1);
    }
    if (strcmp(argv[1], "-p") !=  0){
      exit(1);
    }
    

   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
 
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
      {
      perror("ERROR vytoreni socketu");
      exit(1);
      }
   

   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = atoi(argv[2]);
   if (portno == 0){
    exit(1);
   }
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR bind");
      exit(1);
      }

   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1)
   {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0)
         {
         perror("ERROR nepodaril se accept");
         exit(1);
         }
      
      /* forkujeme */
      int pid = fork();
      if (pid < 0)
         {
         perror("ERROR pri forknuti");
         exit(1);
         }
      
      if (pid == 0)
         {
         close(sockfd);
         obsluha_clienta(newsockfd);
         exit(0);
         }
      else
         {
         close(newsockfd);
         }
   } 
}


void obsluha_clienta (int sock)
{

   
   FILE * f;
   if ((f = fopen("/etc/passwd","r")) == NULL) {
      perror( "soubor nelze otevrit!");
      exit(1);
   }
   //printf("pripojeno \n");
     
   
   int n;
   char buffer[256];
   char zprava[256];
   
   

      
      
   struct Zaznam zaznam; 
   while (1)  {
      bzero(buffer,256);
      memset(zprava, '\0', 256);
      n = read(sock,buffer,255);
      //printf("buffer : |%s|\n", buffer);
      if (n < 0)
      {
      perror("ERROR cteni ze socketu");
      exit(1);
      }
      
      if (strcmp(buffer, "konec") == 0 ) return;
      fseek(f, 0L, SEEK_SET);
      
      while (1){
         if (feof(f)){close(f); break;}
         if (get_zaznam(f, &zaznam) == 1) { // spatny format passwd  
            strcpy(zprava, "!Chyba: chybny format zdrojove databaze\n");
            write(sock, zprava, strlen(zprava));     
            
            close(f);
            return;                
                   
         }
         if (buffer[6] == 'u') {
            if (strcmp(&buffer[7], zaznam.uid) == 0 ){
               //strcat(zprava, zaznam.uid);
               co_chces(buffer, zprava, zaznam)  ;
               n = write(sock,zprava,strlen(zprava));
   
               if (n < 0)
               {
                  perror("ERROR psani do socketu");
                  close(f);
                  return;
               }
               //printf("%s\n", zprava);
            }     
         } else if (buffer[6] == 'l') {
            if (strcasecmp(&buffer[7], zaznam.login) == 0 ){
               //strcat(zprava, zaznam.login);
               co_chces(buffer, zprava, zaznam);  
               n = write(sock,zprava,strlen(zprava));
   
               if (n < 0)
               {
                  perror("ERROR zapis do socketu");
                 close(f);
                  return;
               }               
               //printf("%s\n", zprava);
  
            } 
             
         }
         
         int cc = fgetc(f);
         if (cc == EOF ) break; else ungetc(cc, f);
      }
      //printf("zprava %s \n", zprava);
      if (zprava[0] == '\0'){
         strcpy(zprava, "!Chyba: neznamy ");
         strcat(zprava, ((buffer[6] == 'l')?"login ":"uid ") );
         strcat(zprava, &buffer[7]);
         strcat(zprava, "\n");
         write(sock, zprava, strlen(zprava));
      }
      
   }
   
   close(f);
}

int get_zaznam(FILE *f, struct Zaznam *zaznam) {
   if ( ':' != nacti(f, zaznam->login) ) {return 1;}
   if ( ':' != nacti(f, zaznam->heslo)) {return 1;}
   if ( ':' != nacti(f, zaznam->uid)) {return 1;}
   if ( ':' != nacti(f, zaznam->gid)) {return 1;}
   if ( ':' != nacti(f, zaznam->popis)){return 1;}
   if ( ':' != nacti(f, zaznam->home)) {return 1;}
   //printf("hello from get_zaznam\n");
   int c = nacti(f, zaznam->shell);
   if ( c == '\n' ) return 0;

   return 1  ;
}


int nacti(FILE * f, char *z ){
   int i = 0;
   int c;
   
   while (1) {
      c = fgetc(f);
      if (c == ':' || c == '\n'){ 
         z[i] = '\0';    
         return c;
      }
      z[i++] = c;   
   }
}

void co_chces(char *buffer, char *zprava, struct Zaznam zaznam) {
   int i;
   for (i = 0; i<6; i++) {
               switch(buffer[i]){
               case 'L':
                  strcat(zprava, zaznam.login);
                  strcat(zprava, " "); 
                  break;
               case 'U':
                  strcat(zprava, zaznam.uid);
                  strcat(zprava, " "); 
                  break;
               case 'G':
                  strcat(zprava, zaznam.gid);
                  strcat(zprava, " "); 
                  break;
               case 'N': 
                  strcat(zprava, zaznam.popis);
                  strcat(zprava, " "); 
                  break;
               case 'H':
                  strcat(zprava, zaznam.home);
                  strcat(zprava, " "); 
                  break;
               case 'S':
                  strcat(zprava, zaznam.shell);
                  strcat(zprava, " "); 
                  break;
               }
               
            }
   zprava[strlen(zprava)-1] = '\n'  ;
}   