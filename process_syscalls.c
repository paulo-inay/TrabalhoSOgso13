// João Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

/*

	Este programa exemplifica o uso de 3 syscalls de gerenciamento de processos:
fork, waitpid e exit.
	Ele cria um processo filho e depois um processo filho desse filho. Entao, 
realiza algumas operacoes aritmeticas simples sobre variaveis "comuns" aos processos 
(pai e filhos), a fim de explicitar a independencia entre elas.

*/

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(){
	
	pid_t child, grandchild;
	int A = 13, B = 16, C = 0;
	int status_c;
	
	child = fork(); // Syscall fork --> cria um novo processo (processo filho)
	
	if(child == 0){ /* Processo filho --> possui uma cópia de todas as variaveis do
					processo pai; assim sendo, a partir daqui, qualquer alteracao de
					valor nao influencia as variaveis do pai */
		
		int D = 7;
		
		/* Alteracao das variaveis no processo filho em questao para futura comparacao
			com "as mesmas" do processo pai*/
		A = 14;
		B = 17;
		C = 25;
		
		grandchild = fork();
		
		if(grandchild == 0){ // Processo filho do filho
			
			/* Alteracao das variaveis no processo filho em questao para futura comparacao
			com "as mesmas" do processo pai*/
			A = (B - A);
			B = (C - B);
			C = C * (B-A);
			D = C - (A + B);
			printf("\nProcesso filho do filho: A --> %d | B --> %d | C --> %d | D --> %d", A, B, C, D);
			fflush(stdout);
			_exit(0); // Syscall exit --> encerra o processo
			
		}else{ //Codigo do processo filho
			
			/* Aqui nao foi inserido o waitpid a fim de verificar a condicao de processo orfao
			e processo zumbi, isto eh, quando o processo pai (neste caso, child) termina 
			antes que o filho (neste caso, grandchild) */
			
			
			/* Impressao para constatar a independencia entre as variaveis 
			do processo e do filho, que seriam "as mesmas" */
			printf("\nProcesso filho: A --> %d | B --> %d | C --> %d | D --> %d", A, B, C, D); 
			fflush(stdout);
			_exit(0);
			
		}
		
		
	}else{ // Codigo do processo pai
		
		waitpid(child, &status_c, 0); /* Syscall waitpid --> processo pai
										aguarda o fim da execucao do filho
										para poder executar*/
						
		// Verificacao do status de execucao do processo filho
		printf("\nStatus do filho: %d\n", status_c);
		
		/* Impressao para constatar a independencia entre as variaveis 
		do processo e do filho, que seriam "as mesmas" */
		printf("\nProcesso pai: A --> %d | B --> %d | C --> %d", A, B, C);
		
	}
	
	return 0;
	
}
