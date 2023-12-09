#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#define MSGSIZE 250
typedef int pipe_t[2];         

int main (int argc, char **argv)
{   
    int Q=argc-1;
    char linea[MSGSIZE];
   int pid[Q], i,j,cCar=0,cLin=0; 		/* pid per fork, j per indice, piped per pipe */
   //char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
   int pidFiglio, status, ritorno;      /* per wait padre */
    int fd;             //usato per i file
    if((fd=creat("Camilla",O_WRONLY))<0){
        printf("errore creazione file Camilla");
        exit(1);
    }
	if (argc < 3)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(2);
	}
    // alloco memoria per pipes
    pipe_t* pFigliPadre= (pipe_t*) malloc(Q*sizeof(pipe_t));
    /* si crea una pipe per ogni figlio*/
    for(i=0;i<Q;i++){
        if(pipe(pFigliPadre[i])<0){
            printf("errore nella pipe Figli-Padre");
            exit(3);
        }
        
    } 
    
    //creazione dei Q figli
    for(int q=0;q<Q;q++){
        if ((pid[q] = fork()) < 0)  
	    {
           	printf("Errore creazione figlio\n");
    		exit(4); 
	    }
	    if (pid[q]== 0)  
	    {   
		/* figlio */
		
            for(int p=0;p<Q;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
                close(pFigliPadre[p][0]);
                if(p!=q){ 
                    close(pFigliPadre[p][1]);
                }
            }
            if ((fd = open(argv[q+1], O_RDONLY)) < 0)		/* apriamo il file */
            {   
                printf("Errore in apertura file %s\n", argv[q+1]);
                exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
            }
            while(read(fd,&linea[cCar],1)){
                if(linea[cCar]=='\n'){
                    cCar++;
                    if(cCar<10 && (isdigit(linea[0]))!=0){
                        write(pFigliPadre[q][1],linea,cCar);
                        cLin++;
                    }
                    cCar=0;
                }else cCar++;
            }
		
		exit(cLin);	/* figlio deve tornare al padre  */
        }
    }

	/* padre */
    
    for(int p=0;p<Q;p++){
        close(pFigliPadre[p][1]);
    }
    for(j=0;j<Q;j++){
        cCar=0;
        while(read(pFigliPadre[j][0],&linea[cCar],1)){
            if(linea[cCar]=='\n'){
                linea[cCar+1]='\0';
                cCar++;
                printf("il figlio di indice %d ha inviato la linea del file %s che dice: %s",pid[j],argv[j+1],linea);
                write(fd,linea,cCar);
                cCar=0;
            }else cCar++;
        }
    }
	
	
	/* padre aspetta il figlio */
    for(i=0;i<Q;i++){
        if ((pidFiglio=wait(&status)) < 0)
        {
                printf("Errore wait\n");
                exit(5);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d linee inviate al padre(se 255 problemi!)\n", pidFiglio, ritorno);
        }
    }
	exit(0);
}
