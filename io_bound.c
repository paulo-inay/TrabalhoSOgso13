// João Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(){
	
	int fd, fp, s, i, j;
	char buffer[100], arquivo[15], cripto[25];
	ssize_t n;	
	
	j = 0;
	strcpy(arquivo, "rummi.txt\0");
	strcpy(cripto, "rummi_cripto.txt\0");
	
	while(j < 4){
		
		fd = open(arquivo, O_RDONLY);
		fp = open(cripto, O_CREAT|O_WRONLY);
		
		n = write(STDOUT_FILENO, "\nArquivo a ser criptografado: ", 30);
		n = write(STDOUT_FILENO, arquivo, strlen(arquivo));
		
		n = read(fd, buffer, 100);
		
		// Se n = 0, entao o arquivo chegou ao fim
		// Enquanto nao acabar o arquivo, le 100 bytes, criptografa, e salva no novo arquivo
		while(n > 0){
			
			for(i = 0; i < strlen(buffer); i++){
				buffer[i] += (i % 10);
			}
			
			n = write(fp, buffer, strlen(buffer));
			n = read(fd, buffer, 100);
			
		}
		
		n = write(fp, "\0", 1);
		
		n = write(STDOUT_FILENO, "\nArquivo criptografado! Vide ", 29);
		n = write(STDOUT_FILENO, cripto, strlen(cripto));
		
		s = close(fd);
		s = close(fp);
	
		j++; // Incremento em j para selecionar o proximo arquivo
		
		// Switch-case para selecao do proximo arquivo a ser criptografado
		switch(j){
			
			case 1:
				strcpy(arquivo, "calc.txt\0");
				strcpy(cripto, "calc_cripto.txt\0");
				break;
			case 2:
				strcpy(arquivo, "matriz.txt\0");
				strcpy(cripto, "matriz_cripto.txt\0");
				break;
			case 3:
				strcpy(arquivo, "quarto.txt\0");
				strcpy(cripto, "quarto_cripto.txt\0");
				break;
		}
	
	}
	
			n = write(STDOUT_FILENO, "\n\n", 2);
	
	return 0;
	
}
