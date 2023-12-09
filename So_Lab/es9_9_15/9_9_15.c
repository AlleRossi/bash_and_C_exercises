#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
//#define MSGSIZE 512
typedef int pipe_t[2];          //

int main (int argc, char **argv)
{   
    int N=argc-2;
    char cr,ch;
   int pid[N], i,j,ctr[N]; 		/* pid per fork, j per indice, piped per pipe */
   //char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
   int pidFiglio, status, ritorno;      /* per wait padre */
    int fd;             //usato per i file

	if (argc < 4)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
    // alloco memoria per pipes
    pipe_t *pPadreFigli= (pipe_t*) malloc(N*sizeof(pipe_t));
    pipe_t* pFigliPadre= (pipe_t*) malloc(N*sizeof(pipe_t));
    /* si crea una pipe per ogni figlio*/
    for(i=0;i<N;i++){
        if(pipe(pPadreFigli[i])<0){
            printf("errore nella pipe Padre-Figli");
            exit(2);
        }
        if(pipe(pFigliPadre[i])<0){
            printf("errore nella pipe Figli-Padre");
            exit(2);
        }
        ctr[i]=1;
        
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
		
            for(int p=0;p<N;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
                close(pPadreFigli[p][1]);
                close(pFigliPadre[p][0]);
                if(p!=j){ 
                    close(pFigliPadre[p][1]);
                    close(pPadreFigli[p][0]);
                }
            }
            if ((fd = open(argv[j+1], O_RDONLY)) < 0)		/* apriamo il file */
            {   	printf("Errore in apertura file %s\n", argv[1]);
                        exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
            }
            while(read(pPadreFigli[j][0],&cr,1)){
                if(cr=='t') break;
                read(fd,&cr,1);
                write(pFigliPadre[j][1],&cr,1);
            }
		
		exit(0);	/* figlio deve tornare al padre  */
        }
    }

	/* padre */
    char token='g';
    for(int p=0;p<N;p++){
        close(pFigliPadre[p][1]);
        close(pPadreFigli[p][0]);
    }
    if((fd=open(argv[argc-1],O_RDONLY))<0){
        printf("errore apertura file del padre");
        exit(-1);
    }
	while(read(fd,&cr,1)){
    	for(j=0;j<N;j++){
    	    if(ctr[j]){
    	        write(pPadreFigli[j][1],&token,1);
                read(pFigliPadre[j][0],&ch,1);
                printf("letto carattere %c dal padre\n",cr);
                printf("letto carattere %c dal figlio %d\n",ch,j);
    	        if(ch!=cr){
                    ctr[j]=0;
                    printf("setto a zero ctr di %d\n",j);
                }
            }
       	}
    }
    token='t';
    for(i=0;i<N;i++){
        printf("valore di ctr %d\n",ctr[i]);
        if(ctr[i]!=1){
            kill(pid[i],SIGKILL);
            printf("sto chiudendo forzatamente figlio %d\n",i);
        }else{
            write(pPadreFigli[i][1],&token,1);
        }
    }
	
	/* padre aspetta il figlio */
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
