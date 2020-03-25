// Joao Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691


/*	O programa a seguir demonstra a utilizacao de system calls de gerenciamento de memoria como mmap(), brk(), sbrk() e munmap().
 *	Nao tentamos dar uma tarefa util ao programa pois uma tarefa dessas provavelmente envolveria syscalls de entrada e saida
 *ou leitura e escrita de arquivo.
 *	Portanto o programa vai simplesmente declarar uma string, escrever ela em uma memoria alocada e depois desalocar essa memoria.
 *	A alocacao sera feita de dois jeitos: uma utilizando mmap e a outra utilizando brk.
 */

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<errno.h>

#define PAGE_SIZE 4096 //Pagina de memoria: 4kb.

int main(){
	
	char aventura[22] = "Adventure begins...\n\0";
	char *pointer1;
	char *pointer2;

	/**************************************************************************************/

	//Programa utilizando mmap para a alocacao de memoria.

	pointer1 = (char*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); //Alocacao de memoria usando mmap.

	if(pointer1 != MAP_FAILED){

		for(int i = 0; i < 22; i++){
			pointer1[i] = aventura[i];
		}
		
	} else {

		printf("Erro!\n");
		exit(0);

	}

	munmap(pointer1, PAGE_SIZE); //Desaloca o espaco de memoria alocado.

	/**************************************************************************************/

	//Programa utilizando brk para alocacao de memoria.

	pointer2 = sbrk(0); //Recebe localizacao do program break.

	if(brk(pointer2 + PAGE_SIZE) != -1){ //Aumenta o program break em uma pagina para alocacao de memoria e verifica se a operacao teve sucesso.

		for(int i = 0; i < 22; i++){
			pointer2[i] = aventura[i];
		}

	} else {

		printf("Erro!\n");
		exit(0);

	}

	brk(pointer2); //Desaloca o espaco de memoria alocado.

	/**************************************************************************************/

	return 0;

}
