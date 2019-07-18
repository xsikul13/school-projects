#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   //
   int bflag, ch, fd;
   char host_name[255];
   int H, L, N, S, U, G;
   H = L = N = S = U = G = 0;
 
   char u_or_l = 0;
   char *kdo[32]; ;
   int kolik = 0;
   bflag = 0;
   int pocet_flags = 0;
   int h_flags = 0;
   int p_flags = 0;
   char flags[256];memset(flags, '\0', sizeof(flags));
   int i;
   for (i = 0; i < 6; i++)
      flags[i] = 'X';
   
   while ((ch = getopt(argc, argv, "h:p:u:l:LUGNHS")) != -1) {
             switch (ch) {
             case 'h':
                      
                       h_flags = 1;
                       strcpy(host_name, optarg);
                       //printf("host_name: %s:\n", host_name);
                     break;
             case 'p':
                       p_flags = 1;
                       portno = atoi(optarg);
                       //printf("port: %s:  portno: %d\n", optarg, portno);
                     break;
             case 'u':
                       
                        
                        u_or_l = 'u';
                        kolik = 0;
                        optind--;
                        for( ;optind < argc && *argv[optind] != '-'; optind++, kolik++){
                          //strcat(kdo, argv[optind]);
                          //strcat(kdo, ":");
                          kdo[kolik] = argv[optind];         
                        }
             
                      break;
             case 'l':
                        
                        u_or_l = 'l';
                        kolik = 0;
                        optind--;
                        for( ;optind < argc && *argv[optind] != '-'; optind++, kolik++){
                          //strcat(kdo, argv[optind]);
                          //strcat(kdo, ":");
                          kdo[kolik] = argv[optind];         
                        }
             
                      break;
              case 'L':
                        if (L == 1) break;
                        L = 1;
                        flags[pocet_flags++] = 'L'  ;
                        
                      break;
                      
              case 'U':
                        if (U == 1) break;
                        U = 1;
                        flags[pocet_flags++] = 'U'  ;
                        
                      break;
              case 'G':
                        if (G == 1) break;
                        G = 1;
                        flags[pocet_flags++] = 'G'  ;
                        
                      break;
              case 'N':
                        if (N == 1) break;
                        N = 1;
                        flags[pocet_flags++] = 'N'  ;
                        
                      break;                
              case 'H':
                        if (H == 1) break;
                        H = 1;
                        flags[pocet_flags++] = 'H'   ;
                        
                      break;
              case 'S':
                        if (S == 1) break;
                        S = 1;
                        flags[pocet_flags++] = 'S'     ;
                        
                      break;        
             default:
                     perror("ERROR: parametry");exit(1);
                     break;
             }
     }
     argc -= optind;
     argv += optind;

  
   
   if ( !(h_flags && p_flags && u_or_l) ) {
      perror("error: nezadany povinne arg");
      exit(1);
   }
   //
   if (u_or_l == 0) {
      perror("eRROR: nezadan parametr u nebo l");
      exit( 1);
   }
   flags[6] = u_or_l;
   //kdo[strlen(kdo)-1] = '!';
   
  
               
   
   //printf("flags |%s| \n", flags);
   

   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
   {
      perror("ERROR vytvoreni socketu");
      exit(1);
   }
   server = gethostbyname(host_name);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, nenalezeni serveru\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* připojení k serveru */
   if (connect(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR nepripojili jsme se");
      exit(1);
   }
   

   
   
   
   for  (i = 0; i < kolik; i++){
         memset(&flags[7], '\0', 255-7);
         strcat(&flags[7], kdo[i]);
         //printf("%s\n", flags);
         n = write(sockfd,flags,strlen(flags));
   
         if (n < 0)
         {
            perror("ERROR zapis do socketu");
            exit(1);
         }
         
         
         
          
         char zprava[1024]; 
         bzero(zprava,1024);
         n = read(sockfd,zprava,1024);
   
         if (n < 0)
         {
            perror("ERROR cteni ze socketu");
            exit(1);
         }
         
         if (zprava[0] == '!'){
            fprintf(stderr, "%s", &zprava[1]);
         }else{
            printf("%s", zprava);
         }

         
         
   }
   strcpy(flags, "konec");
   n = write(sockfd,flags,strlen(flags));
   
   if (n < 0)
   {
      perror("ERROR psani do socketu");
      exit(1);
   }

   
  
   return 0;
}