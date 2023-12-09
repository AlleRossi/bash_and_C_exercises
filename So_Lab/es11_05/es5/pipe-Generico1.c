#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>


int main (int argc, char **argv)
{   
   int  j, piped[2][2]; 		/* pid per fork, j per indice, piped per pipe */
   char mess;	/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
   int pidFiglio, status, ritorno;      /* per wait padre */

	if (argc < 3)
	{	printf("Numero dei parametri errato %d: ci vogliono almeno 2 parametri\n", argc);
    		exit(1);
	}
	/* si crea una pipe */
	if (pipe(piped[0]) < 0 )  
	{   	printf("Errore creazione pipe\n");
    		exit(2); 
	}
	if(pipe(piped[1])<0){
		printf("errore pipe 2");
		exit(-2);
	}
	int pids[argc-1];
	for(int i=1;i<argc;i++){
		if((pids[i-1]=fork())<0){
			printf("errore nella fork numero %d",i);
		}
		if (pids[i-1] == 0)  
        	{
                	/* figlio */
                	int fd; /* variabile presente solo nello stack del figlio; va bene anche definirla nel main */
                	close(piped[0][0]);        /* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
			close(piped[1][0]);
                	if ((fd = open(argv[i], O_RDONLY)) < 0)         /* apriamo il file */ 
            		{       	
				printf("Errore in apertura file %s\n", argv[i]);
                        	exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
                	}

			printf("DEBUG-Figlio %d sta per iniziare a scrivere una serie di messaggi\n", getpid());
                	j=0; /* il figlio inizializza la sua variabile j per contare i messaggi che mandera' al padre */
                /* il figlio legge tutto il file passato come parametro */
                	while (read(fd,&mess,1)) /* il contenuto del file e' tale che in mess ci saranno 4 caratteri e il terminatore di linea */
                	{ 
                        	if(i%2!=0){
					if(mess>47 && mess<58){
						write(piped[0][1],&mess,1);
						j++;
					}
				}else{
					if(!(mess>47 && mess <58)){
						write(piped[1][1],&mess,1);
						j++;
					}
				}
                	}
                	printf("DEBUG-Figlio %d scritto %d messaggi sulla pipe\n", getpid(), j);
        	        exit(j);        /* figlio deve tornare al padre il numero di linee lette che corrisponde al numero di stringhe mandate al padre */
	        }

	}

	/* padre */

	close(piped[0][1]); /* il padre CHIUDE il lato di scrittura: sara' quindi il LETTORE della pipe */
	close(piped[1][1]);
	char unit;
	int check=0,check2=0;
	printf("DEBUG-Padre %d sta per iniziare a leggere i messaggi dalla pipe\n", getpid());
	j=0; /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio */
	while (1){
		if(read(piped[0][0],&mess,1)){
			printf("%c",mess);
			check=1;
		}else check=0;
		if(read(piped[1][0],&unit,1)){
			printf("%c",unit);
			check2=1;
		}else check2=0;
		if(check==0 && check2==0) break;
		check=0;
		check=0;
	}
	printf("DEBUG-Padre %d letto %d messaggi dalla pipe\n", getpid(), j);
	/* padre aspetta il figlio */
	if ((pidFiglio=waitpid(pids[0],&status,0)) < 0)
	{
      		printf("Errore wait\n");
      		exit(4);
	}
	waitpid(pids[1],&status,0);
	if ((status & 0xFF) != 0)
        	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
	else
	{
       		ritorno=(int)((status >> 8) & 0xFF);
       		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
	}

	exit(0);
}
