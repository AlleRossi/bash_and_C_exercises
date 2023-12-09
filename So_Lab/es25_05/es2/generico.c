#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#define MSGSIZE 512
typedef int pipe_t[2];

int main (int argc, char **argv)
{   
	int N=argc-1;
   	int pid[N]; 		/* pid per fork, j per indice, piped per pipe */
   //char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
	pipe_t *pipesFigliPadre;
	pipe_t *pipesNipotiPadre;
	int j=0;
	int index;
	char ch[26];
	char cMaiusc[26];
	char cr;
	long int ctr=0;
   	int status, ritorno;      /* per wait padre e figli*/

    for(int p=0;p<26;p++){//riempio ch e cMaiusc con i relativi char, come da tavola ASCII
		ch[p]=97+p;
		cMaiusc[p]=65+p;
	}
	if (argc < 3)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
    // alloco memoria per pipes
	pipesFigliPadre=(pipe_t*) malloc(N*sizeof(pipe_t));
	pipesNipotiPadre=(pipe_t*) malloc(N*sizeof(pipe_t));
	/* si crea una pipe per ogni figlio e nipote*/
    for(int i=0;i<N;i++){
        if(pipe(pipesFigliPadre[i])<0){
            printf("errore nella creazione delle pipe Figli");
            exit(3);
        }
		if(pipe(pipesNipotiPadre[i])<0){
            printf("errore nella creazione delle pipe Nipoti");
            exit(3);
        }
    }
    //creazione dei N figli
    for(j=0;j<N;j++){
        if ((pid[j] = fork()) < 0)  
	    {
           	printf("Errore creazione figlio\n");
    		exit(3); 
	    }
	    if (pid[j] == 0){   
			/* figlio */
			int fd;	/* variabile presente solo nello stack del figlio; va bene anche definirla nel main */
			if ((fd = open(argv[j+1], O_RDWR)) < 0)		/* apriamo il file */
			{   	printf("Errore in apertura file %s\n", argv[1]);
						exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
			}
			for(int p=0;p<N;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
				close(pipesFigliPadre[p][0]);
				if(p!=j){
					 close(pipesFigliPadre[p][1]);
				}
			}
			char space=' ';
			while(read(fd,&cr,1)){
				
				if(isdigit(cr)>0){
					lseek(fd,-1,SEEK_CUR);
					write(fd,&space,1);
					ctr++;
				}
			
			}
			write(pipesFigliPadre[j][1],&ctr,sizeof(int));
			int pidNipoti[N];
			if((pidNipoti[j]=fork())<0){
				printf("Errore creazione Nipote");
				exit(-1);
			}
			if(pidNipoti[j]==0){// codice nipoti
				for(int p=0;p<N;p++){
					close(pipesNipotiPadre[p][0]);
					if(p!=j) close(pipesNipotiPadre[p][1]);
				}
				lseek(fd,0,SEEK_SET);
				int ctrNipoti=0;
				while(read(fd,&cr,1)){
					for(int i=0;i<26;i++){
						if(cr==ch[i]){
							lseek(fd,-1,SEEK_CUR);
							write(fd,&cMaiusc[i],1);
							ctrNipoti++;
						}
					}
				}
				write(pipesNipotiPadre[j][1],&ctrNipoti,sizeof(int));
				index=0;
				while(true){
					if(ctrNipoti<(256+256*index))exit(index);
					index++;
				}
			}

			/* figli aspettano nipoti */
			if ((pidNipoti[j]=wait(&status)) < 0){
				printf("Errore wait\n");
				exit(4);
			}
			if ((status & 0xFF) != 0)
				printf("Figlio con pid %d terminato in modo anomalo\n", pidNipoti[j]);
			else
			{
				ritorno=(int)((status >> 8) & 0xFF);
				printf("Il nipote con pid=%d ha ritornato %d (se 255 problemi!)\n", pidNipoti[j], ritorno);
			}
	
			index=0;/* figlio deve tornare al padre  */
			while(true){
				if(ctr<(256+256*index))exit(index);
				index++;
			}
	    }
    }

	/* padre */
	for(int i=0;i<N;i++){		//chiudo tutte le pipe che non servono al padre
		close(pipesFigliPadre[i][1]);
		close(pipesNipotiPadre[i][1]);
	}
	int numFiglio;
	int numNipote;
	for(int i=0;i<N;i++){
		read(pipesFigliPadre[i][0],&numFiglio,sizeof(int));
		read(pipesNipotiPadre[i][0],&numNipote,sizeof(int));
		printf("Il figlio e Nipote relativo al file %s hanno ritornato %d e %d\n",argv[i+1],numFiglio,numNipote);
	}

	/* padre aspetta il figlio */
	for(int i=0;i<N;i++){
		if ((pid[i]=wait(&status)) < 0){
			printf("Errore wait\n");
			exit(4);
		}
		if ((status & 0xFF) != 0)
			printf("Figlio con pid %d terminato in modo anomalo\n", pid[i]);
		else
		{
			ritorno=(int)((status >> 8) & 0xFF);
			printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pid[i], ritorno);
		}
	}

	exit(0);
}
