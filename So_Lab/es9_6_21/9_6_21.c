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
    int N=argc-1;
    int fc=creat("/home/kali/Prova/So_Lab/SecondHalf/esMar/es9_6_21/alessandrorossi",O_WRONLY);
    if(fc<0){
        printf("errore creazione del file temporaneo");
        exit(-1);
    }
    int pid[N], i,j; 		/* pid per fork, j per indice, piped per pipe */
    //char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
    int pidFiglio, status, ritorno;      /* per wait padre */
    int fd;             //usato per i file
    char linea[200];
    int linelenght=0;
    pipe_t pipeSpeciale;
    if(pipe(pipeSpeciale)<0){
        printf("errore creazione pipe speciale");
        exit(-2);
    }
	if (argc < 3)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
    //creo figlio speciale che ritorna il numero di linee
    int pids;
    if((pids=fork())<0){
        printf("errore creazione figlio speciale");
        exit(2);
    }
    if(pids==0){
        close(pipeSpeciale[0]);
        close(1);
        dup(pipeSpeciale[1]);//ridirigo l'output di wc -l a questa pipe per leggerlo in seguito
        close(2);
        open("/tmp/null",O_WRONLY);     //ridirigo lo standard error qui in modo da non avere stampe a terminale in caso di errori
        execlp("wc","wc","-l",argv[1],(char *)0);
        exit(-1);   //in caso di errori ritorno -1: NON SI DOVREBBE ARRIVARE QUI
    }
    close(pipeSpeciale[1]);
    char ch[255];
    i=0;
    while(read(pipeSpeciale[0],&ch[i],1)){
        i++;
    }
    int NumLinee;
    if(i!=0){
        ch[i-1]='\0';
        NumLinee=atoi(ch);
        printf("il padre ha ricevuto %d come numero di linee\n",NumLinee);
    }
    close(pipeSpeciale[0]);
    // alloco memoria per pipes
    pipe_t* pFigliPadre= (pipe_t*) malloc(N*sizeof(pipe_t));
    /* si crea una pipe per ogni figlio*/
    for(i=0;i<N;i++){
        if(pipe(pFigliPadre[i])<0){
            printf("errore nella pipe Figli-Padre");
            exit(23);
        }
        
    } 
    
    //creazione dei N figli
    for(j=0;j<N;j++){
        if ((pid[j] = fork()) < 0)  
	    {
           	printf("Errore creazione figlio\n");
    		exit(4); 
	    }
	    if (pid[j]== 0)  
	    {   
		/* figlio */
		
            for(int p=0;p<N;p++){ 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
                close(pFigliPadre[p][0]);
                if(p!=j){ 
                    close(pFigliPadre[p][1]);
                }
            }
            if ((fd = open(argv[j+1], O_RDONLY)) < 0)		/* apriamo il file */
            {   	printf("Errore in apertura file %s\n", argv[1]);
                        exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
            }
            while(read(fd,&linea[linelenght],1)){
                if(linea[linelenght]=='\n'){
                    linelenght++;
                    write(pFigliPadre[j][1],&linelenght,sizeof(int));
                    write(pFigliPadre[j][1],linea,linelenght);
                    ritorno=linelenght;
                    linelenght=0;
                }else linelenght++;
            }
		
		exit(ritorno);	/* figlio deve tornare al padre  */
        }
    }

	/* padre */
    for(int p=0;p<N;p++){
        close(pFigliPadre[p][1]);
    }
    for(j=0;j<NumLinee;j++){
        for(i=0;i<N;i++){
            read(pFigliPadre[i][0],&linelenght,sizeof(int));
            printf("il padre ha letto lunghezza linea %d   \n",linelenght);
            read(pFigliPadre[i][0],linea,linelenght);
            //printf("il padre ha letto linea: %s   \n",linea);
            write(fc,linea,linelenght);
        }
    }
	
	
	/* padre aspetta il figlio */
    for(i=0;i<N+1;i++){
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
                if(pidFiglio==pids){
                    printf("Il figlio speciale con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
                }else{
                    printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
                }
        }
    }
	exit(0);
}
