#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#define MSGSIZE 512

typedef int pp[2];

int main (int argc, char **argv)
{
	int pid,status,pidF;

	pp* piped = malloc((argc - 1) * sizeof(pp));

	if(argc<3){
		printf("numero parametri insufficente ");
		exit(1);
	}
	for(int j=0;j<argc-1;j++){
		if(pipe(piped[j])<0){
			printf("errore nella pipe");
			exit(3);
		}
	}
	long int tot;
	for(int i=0;i<argc-1;i++){
		if((pid=fork())<0){
			printf("errore nella fork");
			exit(2);
		}
		if(pid==0){
			int ppiped[2];
			for(int p=0;p<argc-1;p++){
				if(p!=i){
				close(piped[p][1]);
				close(piped[p][0]);
				}
			}
			if(pipe(ppiped)<0){
				printf("errore creazione seconda pipe");
				exit(6);
			}
			printf("il figlio %d sta per creare un nipote\n",getpid());
			int pidd;
			if((pidd=fork())<0){
				printf("errore creazione nipote");
				exit(4);
			}
			if(pidd==0){
				int fd;
				close(0);
				close(ppiped[0]);
				if((fd=open(argv[i+1],O_RDONLY))<0){
					printf("errore nella open");
					exit(5);
				}
				close(1);
				dup(ppiped[1]);
				execlp("wc","wc","-l",(char *) 0);
				exit(-5);
			}
			char x;
			int num=0,tmp;
			close(ppiped[1]);
			while(read(ppiped[0],&x,1)){
				tmp=atoi(&x);
				num=num*10;
				num=num+tmp;
			}
			num=num/10;
			printf("numero finale %d del ciclo %i\n",num,i);
			write(piped[i][1],&num,sizeof(num));
			exit(num);
		}
		int val;
		close(piped[i][1]);
		if(read(piped[i][0],&val,sizeof(int))){
			printf("il numero letto dal padre è %d\n",val);
		}
		printf("il figlio con pid %d ha ritornato %d\n",pid,val);
		tot=tot+val;
		if((pidF=wait(&status)) < 0)
                {
                        printf("errore nella wait");
                        exit(-2);
                }
                if((status &0xFF) !=0){
                        printf("errore anomalo in filgio");
                }

	}
	printf("il numero totale di linee è %ld\n",tot);
	exit(0);
}
