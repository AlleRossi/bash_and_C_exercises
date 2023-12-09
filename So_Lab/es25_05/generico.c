#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#define MSGSIZE 512
typedef int pipe_t[2];
typedef struct{
	char v1;
	long int v2;
} data;
void bubbleSort(data v[]){ 
	int i; bool ordinato = false;
	int dim=26;
	long int tmp;
	char tmpc;
	while (dim>1 && !ordinato){ 
		ordinato = true; /* hp: è ordinato */
		for (i=0; i<dim-1; i++){
			if ((v[i].v2)>v[i+1].v2){
				tmp=v[i].v2;
				v[i].v2=v[i+1].v2;
				v[i+1].v2=tmp;
				tmpc = v[i].v1;
				v[i].v1=v[i+1].v1;
				v[i+1].v1=tmpc;
				ordinato = false;
			}
			dim--;
		}
	}
}


int main (int argc, char **argv)
{   
   int pid[26], j; 		/* pid per fork, j per indice, piped per pipe */
   //char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
	pipe_t pipes[26];
	data info[26];
	char ch[26];
	char cr;
	long int ctr=0;
   	int status, ritorno;      /* per wait padre */
    for(int p=0;p<26;p++){//riempio il campo v1 con i relativi char, come da tavola ASCII
		info[p].v1=97+p;
		ch[p]=97+p;
	}
	if (argc < 2)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
    // alloco memoria per pipes

	/* si crea una pipe per ogni figlio*/
    for(int i=0;i<26;i++){
        if(pipe(pipes[i])<0){
            printf("errore nella creazione delle pipe");
            exit(3);
        }
    }
    //creazione dei N figli
    for(j=0;j<26;j++){
        if ((pid[j] = fork()) < 0)  
	    {
           	printf("Errore creazione figlio\n");
    		exit(3); 
	    }
	    if (pid[j] == 0){   
			/* figlio */
			int fd;	/* variabile presente solo nello stack del figlio; va bene anche definirla nel main */
			if ((fd = open(argv[1], O_RDONLY)) < 0)		/* apriamo il file */
			{   	printf("Errore in apertura file %s\n", argv[1]);
						exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
			}
			if(j!=0){
				for(int p=0;p<argc-2;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
					if(p!=(j-1)) close(pipes[p][0]);
					if(p!=j) close(pipes[p][1]);
				}
			}else{
				for(int p=0;p<26;p++){
					close(pipes[p][0]);
					if(p!=j) close(pipes[p][1]);
				}
			}
			while(read(fd,&cr,1)){
				if(cr==info[j].v1)ctr++;
				//printf("%c\n",info[j].v1)
			}
			if(j==0){
				info[j].v2=ctr;
				write(pipes[j][1],&info,sizeof(info));
			}else{
				read(pipes[j-1][0],&info,sizeof(info));
				info[j].v2=ctr;
				write(pipes[j][1],&info,sizeof(info));
			}
			
			exit(ctr);	/* figlio deve tornare al padre  */
	    }
    }

	/* padre */
	for(int i=0;i<26;i++){		//chiudo tutte le pipe che non servono al padre
		close(pipes[i][1]);
		if(i!=25) close(pipes[i][0]);
	}
	read(pipes[25][0],&info,sizeof(info));
	bubbleSort(info);
	for(int i=0;i<26;i++){
		for(j=0;j<26;j++){
			if(ch[i]==info[j].v1){
				printf("Il figlio di pid %d e lettera %c ha trovato %ld caratteri\n",pid[i],ch[i],info[j].v2);
			}
		}
	}

	/* padre aspetta il figlio */
	for(int i=0;i<26;i++){
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
