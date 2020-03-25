// João Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(){
	
	int fd, fp, s, i;
	char buffer[500];
	ssize_t n;	
	
	/* Syscall open --> abre um arquivo para leitura, escrita ou ambos, retornando o descritor do arquivo */
	fd = open("entrada.txt", O_RDONLY); // Arquivo "entrada.txt" aberto para leitura apenas	
	fp = open("cripto.txt", O_CREAT|O_WRONLY); // Arquivo "cripto.txt" criado (caso não exista) ou aberto para escrita
	
	
	/* Syscall read --> le nbytes de um arquivo para um buffer, retornando o numero de bytes lido */
	n = read(fd, buffer, 500);
	
	
	/* Syscall write --> escreve nbytes de um buffer para um arquivo, retornando o numero de bytes escrito*/
	n = write(STDOUT_FILENO, "\nArquivo a ser criptografado:\n\n", 31);																	
	n = write(STDOUT_FILENO, buffer, strlen(buffer) - 1);
	
	
	// Criptografa o arquivo
	for(i = 0; i < strlen(buffer); i++){
		buffer[i] += (i % 10);
	}
	buffer[i] = '\0';
	
	
	n = write(fp, buffer, strlen(buffer));
	
	
	/* Syscall close --> fecha um arquivo, retornando 0 se executada com sucesso, ou 1 em caso de erro */
	s = close(fd);
	s = close(fp);
	
	n = write(STDOUT_FILENO, "\nArquivo criptografado!\n\n", 25);
	
	return 0;
	
}
