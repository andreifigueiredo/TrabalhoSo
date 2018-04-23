#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
int main(){
	int process;
	char line[100];

	for (;;) {
		printf("cmd: ");
		if (gets(line) == (char *) NULL) /* inserir comando */
			return 0;
		process = fork (); /* cria um novo processo*/
		
		if (process > 0) /* pai */
			wait ((int *) 0); /* aguarda */
		else if (process == 0) { /* filho */
			execlp (line, line, (char *) NULL); /* executa processo */
			printf ("Não pode executar %s\n", line);
			return 1; 
		}
		else if (process == -1) { /* nao pode criar um novo processo */
			printf ("Não foi possivel criar um processo filho!\n");
			return 2; 
		}
 	}
 }