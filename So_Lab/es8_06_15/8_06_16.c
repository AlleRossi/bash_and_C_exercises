#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#define MSGSIZE 512
typedef int pipe_t[2];          //

int main (int argc, char **argv)
{   
   int pid, j; 		/* pid per fork, j per indice, piped per pipe */
   //char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
   int pidFiglio, status, ritorno;      /* per wait padre */
    int N=atoi(argv[argc-1]);
    if(N<=0){
        printf("numero linee non accettabile");
        exit(4);
    }
	if (argc < 4)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
    // alloco memoria per pipes
    pipe_t *piped= (pipe_t*) malloc(N*sizeof(pipe_t));
    if(piped==NULL){
        printf("errore nella malloc");
        exit(2);
    } 
	/* si crea una pipe per ogni figlio*/
    for(int i=0;i<argc-2;i++){
        if(pipe(piped[i])<0){
            printf("errore nella creazione delle pipe");
            exit(3);
        }
    }
    //creazione dei N figli
    for(j=0;j<argc-2;j++){
        if ((pid = fork()) < 0)  
	    {
           	printf("Errore creazione figlio\n");
    		exit(3); 
	    }
	    if (pid == 0)  
	    {   
		/* figlio */
		int fd;	/* variabile presente solo nello stack del figlio; va bene anche definirla nel main */
		for(int p=0;p<argc-2;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
            close(piped[p][0]);
            if(p!=j) close(piped[p][1]);
        }
		if ((fd = open(argv[j+1], O_RDONLY)) < 0)		/* apriamo il file */
		{   	printf("Errore in apertura file %s\n", argv[1]);
            		exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
		}
		
		
		exit(j);	/* figlio deve tornare al padre  */
	    }
    }

	/* padre */
	
	printf("DEBUG-Padre %d letto %d messaggi dalla pipe\n", getpid(), j);
	/* padre aspetta il figlio */
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

	exit(0);
}