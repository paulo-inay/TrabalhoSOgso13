// João Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

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
		
		grandchild = fork(); // Syscall fork --> cria um novo processo (processo filho)
		
		if(grandchild == 0){ // Processo filho do filho
			
			/* Alteracao das variaveis no processo filho em questao para futura comparacao
			com "as mesmas" do processo pai*/
			A = (B - A);
			B = (C - B);
			C = C * (B-A);
			D = C - (A + B);
			printf("Processo filho do filho: A --> %d | B --> %d | C --> %d | D --> %d", A, B, C, D);
			_exit(0);
			
		}else{ //Codigo do processo filho
			
			/* Aqui nao foi inserido o waitpid a fim de verificar a condicao de processo orfao,
			isto eh, quando o processo pai (neste caso, child) termina antes que o filho (neste
			caso, grandchild) */
			
			
			/* Impressao para constatar a independencia entre as variaveis 
			do processo e do filho, que seriam "as mesmas" */
			printf("Processo filho: A --> %d | B --> %d | C --> %d | D --> %d", A, B, C, D); 
																 
			_exit(0);
			
		}
		
		
	}else{ // Codigo do processo pai
		
		waitpid(child, &status_c, 0); /* Syscall waitpid --> processo pai
										aguarda o fim da execucao do filho
										para poder executar*/
						
		// Verificacao do status de execucao do processo filho
		printf("Status do filho: %d\n", status_c);
		
		/* Impressao para constatar a independencia entre as variaveis 
		do processo e do filho, que seriam "as mesmas" */
		printf("Processo pai: A --> %d | B --> %d | C --> %d", A, B, C);
		
	}
	
	return 0;
	
}
