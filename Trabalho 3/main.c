// Sistemas Operacionais - Trabalho 3 - Gerenciamento de memoria
//
// Joao Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "estDados.h"
#include "memGen.h"

int main(){

    /** Declaracao de Variaveis **/

	Instruc entrada;						// Dados de entrada
	procList *li;                           // Lista de processos
	Clock *relogio;                         // Relogio para o algoritmo Clock
	Elem *processo;                         // Ponteiro para processos.
	instrucFila *fila_susp;                 // Fila de instrucoes vindas de processos suspensos
	instrucFila *fila_proc;                 // Fila de instrucoes a serem executadas de um processo que voltou da suspensao.
	Instruc *instrucao;                     // Ponteiro para instrucoes.
	int *RAM;								// Memoria principal
	int tamPage = 4;						// Tamanho das paginas e das molduras
	int tamEnd = 8;							// Tamanho em bits do endereco logico
	int tamRAM = 32;						// Tamanho da RAM
	int tamHD = 64;							// Tamanho da memoria secundaria
	int contRAM = 0;						// Contador para qtd de espacos ocupados na RAM
	int contTemp = 0;                       // Tempo utilizado para a volta de processos inativos.
	FILE *fp;                               // Ponteiro para arquivo
	char leitura[100];                      // Variavel utilizada para leitura do arquivo.
	char *flag;                             // Variavel utilizada para manter o loop enquanto o arquivo nao acabar.


	printf(">>> SIMULADOR DE GERENCIAMENTO DE MEMORIA VIRTUAL <<<\n\n\n");

	/** Configuracoes de Entrada **/

	printf("> > Configuracoes de Entrada\n");
	printf("Insira o tamanho da moldura:\n");
	while(1){
        scanf("%d", &tamPage);
        if(tamPage <= 0){
            printf("Tamanho invalido, insira um numero maior que zero!\n");
        } else break;
	}
	printf("Insira o tamanho da RAM em numero de paginas:\n");
	while(1){
        scanf("%d", &tamRAM);
        if(tamRAM <= 0){
            printf("Tamanho invalido, insira um numero maior que zero!\n");
        } else break;
	}
	printf("Insira o tamanho da memoria secundaria em numero de paginas (deve ser maior que 1.25*tamanho da RAM):\n");
	while(1){
        scanf("%d", &tamHD);
        if(tamHD <= 1.25*tamRAM){
            printf("Tamanho invalido, insira um numero maior que 1.25*tamanho da RAM!\n");
        } else break;
	}
	printf("Insira o tamanho do endereco de memoria da entrada (deve ser grande o suficiente para referenciar toda a RAM):\n");
	while(1){
        scanf("%d", &tamEnd);
        if(pow(2.0, tamEnd) <= tamRAM * tamPage){
            printf("Tamanho invalido, insira um maior.\n");
        } else break;
	}

	/** Alocacao de Memoria ~~ Abertura do Arquivo **/

	RAM = (int*) calloc(tamRAM, sizeof(int));
	if(!RAM){
        printf("Erro durante a alocacao de memoria.\n");
        exit(1);
	}
    li = cria_lista();
    if(!li){
        printf("Erro durante a alocacao de memoria.\n");
        exit(1);
    }
    relogio = cria_relogio();
    if(!relogio){
        printf("Erro durante a alocacao de memoria.\n");
        exit(1);
    }
    fila_susp = cria_fila();
    if(!fila_susp){
        printf("Erro durante a alocacao de memoria.\n");
        exit(1);
    }
    fila_proc = cria_fila();
    if(!fila_proc){
        printf("Erro durante a alocacao de memoria.\n");
        exit(1);
    }
    entrada.end = (char*)calloc(tamEnd + 1,sizeof(char));
    if(!entrada.end){
        printf("Erro durante a alocacao de memoria.\n");
        exit(1);
    }
	fp = fopen("arquivo.txt", "r");
	if(fp == NULL){
		printf("Erro na abertura do aquivo.");
		exit(1);
	}



	/** Loop de Entrada do Arquivo **/

	flag = fgets(leitura,99,fp); //Se o final do arquivo for atingido, fgets retornara nulo.

	while(flag){

        sscanf(leitura, "%d %c", &entrada.proc, &entrada.operation);

		switch(entrada.operation){ //Para os casos C, eh alocado espaco para o novo processo.
                                   //Para W e R, a MMU eh chamada.
                                   //Para I e P, o tratamento eh feito na propria main.
			case 'C':
			    sscanf(leitura, "%d %c %d", &entrada.proc, &entrada.operation, &entrada.tam);
			    printf("\n> > Instrucao // Numero do Processo: %d, Operacao: %c, Numero de Paginas: %d\n", entrada.proc, entrada.operation, entrada.tam);
				alocar_espaco(li, entrada.tam, entrada.proc, tamRAM, RAM, &contRAM, relogio, tamHD);
				break;
			case 'R':
				sscanf(leitura, "%d %c %s\n", &entrada.proc, &entrada.operation, entrada.end);
				printf("\n> > Instrucao // Numero do Processo: %d, Operacao: %c, Endereco: %s\n", entrada.proc, entrada.operation, entrada.end);
				entrada.tam = 0;
				mmu(li, entrada, tamEnd, RAM, tamRAM, &contRAM, relogio, tamPage, fila_susp);
				break;
			case 'W':
				sscanf(leitura, "%d %c %s\n", &entrada.proc, &entrada.operation, entrada.end);
				printf("\n> > Instrucao // Numero do Processo: %d, Operacao: %c, Endereco: %s\n", entrada.proc, entrada.operation, entrada.end);
				entrada.tam = 0;
				mmu(li, entrada, tamEnd, RAM, tamRAM, &contRAM, relogio, tamPage, fila_susp);
				break;
			case 'P':
			    printf("\n> > Instrucao // Numero do Processo: %d, Operacao: %c\n", entrada.proc, entrada.operation);
				processo = busca_processo(li, entrada.proc);
				if(processo){
                    if(processo->susp == 1){
                        printf("Processo %d esta suspenso. Instrucao sera adicionada na fila.\n", entrada.proc);
                        strcpy(entrada.end, "");
                        insere_fila_inicio(fila_susp, entrada, 0);
                    }
                    else {
                        printf("Processo %d executou uma instrucao de CPU.\n", entrada.proc);
                        processo->cpuBound++;
                    }
				} else {
                    printf("Processo %d nao existe...\n", entrada.proc);
				}
				break;
			case 'I':
			    printf("\n> > Instrucao // Numero do Processo: %d, Operacao: %c\n", entrada.proc, entrada.operation);
				processo = busca_processo(li, entrada.proc);
				if(processo){
                    if(processo->susp == 1){
                        printf("Processo %d esta suspenso. Instrucao sera adicionada na fila.\n", entrada.proc);
                        strcpy(entrada.end, "");
                        insere_fila_inicio(fila_susp, entrada, 0);
                    }
                    else {
                        printf("Processo %d executou uma instrucao de IO.\n", entrada.proc);
                        processo->ioBound++;
                    }
				} else {
                    printf("Processo %d nao existe...\n", entrada.proc);
				}
				break;
		}

		printf("\n> > Inicio da impressao dos processos.\n");
		printa_processo(li);

		/** Processos Suspensos ou Processos que Foram Removidos da Suspensao **/

		printf("\n> > Inicio da verificacao de processos suspensos.\n");

		instrucao = busca_fila_final(fila_proc);
        if(instrucao){ //Verifica se a fila de instrucoes de um processo retirado da suspensao possui elementos.
            processo = busca_processo(li, instrucao->proc);
            if(processo){
                printf("Continuacao da execucao de instrucoes do processo retirado da suspensao.\n");
                if(instrucao->end){
                    strcpy(entrada.end, instrucao->end);
                    sprintf(leitura, "%d %c %s", instrucao->proc, instrucao->operation, entrada.end);
                } else sprintf(leitura, "%d %c", instrucao->proc, instrucao->operation);
                remove_fila_final(fila_proc);
                continue; //Loop eh recomecado caso uma instrucao tenha sido encontrada.
            } else {
                printf("O processo nao foi encontrado...\n");
                while(remove_fila_final(fila_proc)); //Remove todos os elementos de fila_proc caso o processo nao exista.
            }
        }

		if(verifica_susp(li)){ //Verifica se ha processos suspensos.
            if(*fila_susp && (contTemp % 10) == 9){ // O escalonador ativa sempre que o resto da divisao da contagem de tempo
                                                    //por 10 for igual a 9.
                printf("Escalonador de processos ativado devido a presenca de instrucoes de processos suspensos.\n");
                instrucao = busca_fila_final(fila_susp);

                if(instrucao->end){
                    strcpy(entrada.end, instrucao->end);
                    sprintf(leitura, "%d %c %s", instrucao->proc, instrucao->operation, entrada.end);
                } else sprintf(leitura, "%d %c", instrucao->proc, instrucao->operation);
                int tamanho = busca_processo(li, instrucao->proc)->sizeTable;

                printf("Processo %d sera retirado da suspensao.\n", instrucao->proc);

                alocar_espaco(li, tamanho, instrucao->proc, tamRAM, RAM, &contRAM, relogio, tamHD);

                criar_fila_proc(fila_susp, fila_proc, instrucao->proc); //Uma nova fila sera criada com as instrucoes
                                                                        //do processo retirado da suspensao.

                remove_fila_final(fila_proc); //Remove instrucao final da fila, pois ela sera executada a seguir.

				contTemp++;
                continue; //Loop recomecado para tratar do processo retirado da suspensao.
            }
            contTemp++;
            printf("Nao ha instrucoes sobrando de processos que voltaram a suspensao e nenhum processo saira da suspensao.\n");
        } else contTemp = 0;

        flag = fgets(leitura,99,fp);

	}

	libera_lista(li);
	libera_fila(fila_susp);
	libera_fila(fila_proc);
	libera_relogio(relogio);
	free(RAM);
	fclose(fp);

	return 0;

}
