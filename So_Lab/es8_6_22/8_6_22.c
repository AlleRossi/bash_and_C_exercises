#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#define MSGSIZE 5
typedef int pipe_t[2];

int main (int argc, char **argv)
{   
    int N=argc-1;
    char buffer[MSGSIZE],text[MSGSIZE];
    int pid[N], i,j; 		/* pid per fork, j per indice, piped per pipe */
    int pidFiglio, status, ritorno;      /* per wait padre */
    int fd;             //usato per i file

	if (argc < 4)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
    // alloco memoria per pipes
    pipe_t *pPadreFigli= (pipe_t*) malloc((N-1)*sizeof(pipe_t));
    /* si crea una pipe per ogni figlio*/
    for(i=0;i<N-1;i++){
        if(pipe(pPadreFigli[i])<0){
            printf("errore nella pipe Padre-Figli");
            exit(2);
        }
        
        
    } 
    
    //creazione dei N figli
    for(j=0;j<N;j++){
        if ((pid[j] = fork()) < 0)  
	    {
           	printf("Errore creazione figlio\n");
    		exit(3); 
	    }
	    if (pid[j]== 0)  
	    {   
		/* figlio */
            fd=open(argv[j+1],O_RDONLY);
            if(j==0){
                for(int p=0;p<N;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
                    close(pPadreFigli[p][0]);
                }
            }else{
                for(int p=0;p<N;p++){ 	/* il figlio CHIUDE il lato di scrittura: sara' quindi lo lettore della pipe */
                    close(pPadreFigli[p][1]);
                    if(p!=j-1){ 
                        close(pPadreFigli[p][0]);
                    }
                }
            }
            if(j==0){
                while(read(fd,buffer,MSGSIZE)){
                    buffer[MSGSIZE-1]='\0';
                    for(int p=0;p<N-1;p++){
                        write(pPadreFigli[p][1],buffer,MSGSIZE);
                    }
                }
            }else{
                while(read(pPadreFigli[j-1][0],buffer,MSGSIZE)){
                    while(read(fd,text,MSGSIZE)){
                        text[MSGSIZE-1]='\0';
                        int pidn;
                        if((pidn=fork())<0){
                            printf("errore creazione nipote");
                        }
                        if(pidn==0){
                            close(pPadreFigli[j-1][0]);
                            close(1);
                            open("dev/null",O_WRONLY);
                            close(2);
                            open("dev/null",O_WRONLY);
                            execlp("diff","diff",buffer,text,(char *)0);
                            //non ci dovrebbe arrivare mai, ma in caso di errore ritorniamo -1
                            exit(-1);
                        }
                        //faccio wait dei nipoti
                        if ((pidFiglio=wait(&status)) < 0)
                        {
                            printf("Errore wait\n");
                            exit(4);
                        }
                        if ((status & 0xFF) != 0)
                            printf("Nipote con pid %d terminato in modo anomalo\n", pidFiglio);
                        else
                        {
                            ritorno=(int)((status >> 8) & 0xFF);
                            if(ritorno==0){
                                printf("Il nipote con pid=%d dice che %s e %s sono uguali\n", pidFiglio,buffer,text);
                            }else printf("Il nipote con pid=%d dice che %s e %s sono diversi\n", pidFiglio,buffer,text);
                        }
                    }
                    lseek(fd,0,0);
                }

            }
		    exit(j);	/* figlio deve tornare al padre  */
        }
    }


    for(int p=0;p<N;p++){
        close(pPadreFigli[p][1]);
        close(pPadreFigli[p][0]);
    }
    
	
	/* padre aspetta i figli */
    for(i=0;i<N;i++){
        if ((pidFiglio=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(4);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
        }
    }
	exit(0);
}
