// João Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

/*	O programa a seguir resolve um numero NUMBER de sistemas lineares de grau DEGREE pelo metodo de eliminacao de gauss.
 *	Nosso objetivo com o trabalho nao eh mostrar o metodo de eliminacao de gauss mas sim criar um codigo que poderia ser considerado CPU-bound.
 *	Portanto o metodo de eliminacao de gauss nao sera explicado, ate por ser complicado de verifica-lo matematicamente.
 *	Decidimos nao utilizar alocacao de memoria nesse codigo para evitar o efeito que syscalls de gerenciamento de memoria (mmap e brk)
 *teriam no tempo util do processo.
 */

#include<stdio.h>
#include<stdlib.h>

#define DEGREE 196
#define NUMBER 5

void *eliminacao_de_gauss(double numbers[][DEGREE+1], double result[]){

	int i, j, k;
	double m;
	double aux;

	for(i = 0; i < DEGREE; i++){

		for(j = 0; j < DEGREE; j++){

			if (j > i){
				m = numbers[j][i] / numbers[i][i];
				for(k = 0; k < DEGREE + 1; k++){
					numbers[j][k] = numbers[j][k] - (m*numbers[i][k]);
				}
			}

		}

	}

	result[DEGREE-1] = numbers[DEGREE-1][DEGREE] / numbers[DEGREE-1][DEGREE-1];

	for(i = DEGREE - 2; i >= 0; i--)
    {
        aux = 0;
        for(j = i; j < DEGREE; j++)
        {
            aux = aux + (numbers[i][j] * result[j]);
        }
        result[i] = (numbers[i][DEGREE] - aux) / numbers[i][i];
    }

}

int main() {

	FILE *fd;
	FILE *fw;

	int i, j, c; //Variaveis para contadores

	double numbers[DEGREE][DEGREE + 1]; //Vetor com todos os numeros do sistema linear.
	double result[DEGREE]; //Vetor com o resultado do sistema linear.

	char title1[8] = "ls1.txt"; //Nome do arquivo de entrada.
	char title2[14] = "output1.txt"; //Nome do arquivo de saida.

	for(c = 0; c < NUMBER; c++){

		title1[2] = c + 49; //muda o valor do caracter com numero no nome do arquivo
		title2[6] = c + 49; //o valor dependera do loop em que nos encontramos

		fd = fopen(title1, "r"); //abre arquivo lsn.txt para leitura

		if(fd == NULL){
			printf("Erro na abertura do arquivo!");
			return 0;
		}

		fscanf(fd, "comeco "); //remove a string "comeco " do inicio do arquivo

		for(i = 0; i < DEGREE; i++){ //loops para a leitura de todos os numeros

			for(j = 0; j < DEGREE + 1; j++){
				fscanf(fd, "%lf ", &numbers[i][j]);
			}

		}

		eliminacao_de_gauss(numbers, result);

		fclose(fd);

		fw = fopen(title2, "w"); //abre arquivo outputn.txt para escrita

		if(fw == NULL){
			printf("Erro na abertura do arquivo!");
			return 0;
		}

		for(i = 0; i < DEGREE; i++){
			fprintf(fw, "x%d: %lf\n", i + 1, result[i]);  //escrita dos resultados em um arquivo
		}

		fclose(fw);

	}


}
