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

/* * * * * * * * * * Funcoes proprias do simulador * * * * * * * * * */

/* Funcao clock_algorithm --> Identifica a pagina que sera substituida usando o Algoritmo do Rel�gio

	Entrada --> o RELOGIO
	Saida --> Moldura de pagina que sera substituida

*/
int clock_algorithm(Clock *li){

	procRAM *ponteiro;
	int frame;

    ponteiro = *li;

    while(ponteiro->R != 0){
    	ponteiro->R = 0;
        ponteiro = ponteiro->prox;
    }

    frame = ponteiro->framePage;

    remove_relogio(li, frame);

	return frame;

}

/* Funcao suspender_processo --> para retirar da RAM e do RELOGIO as paginas de um processo que sera suspenso

	Entrada --> Numero do processo, lista de processos, a RAM, o RELOGIO e o contador da qtd de espacos ocupados na RAM
	Saida --> Nenhuma

*/
void suspender_processo(int processo, procList *li, int *RAM, Clock *relogio, int *contRAM){

	Elem *aux;
	int i = 0;

	// Achar o processo na lista
	aux = busca_processo(li, processo);
	// Percorrer a tabela de paginas, buscando as paginas presentes na memoria
	for(i = 0; i < aux->sizeTable; i++){
		if(aux->pageTable[i].p_a == 1){
			// Remover da RAM e do RELOGIO
			RAM[aux->pageTable[i].pageFrame] = 0;
			(*contRAM)--;
			remove_relogio(relogio, aux->pageTable[i].pageFrame);
			// Atualiza a tabela de paginas
			aux->pageTable[i].pageFrame = -1;
			aux->pageTable[i].p_a = 0;
			if(aux->pageTable[i].M == 1)
				printf("\nPagina do processo que saiu da RAM foi modificada. Salvando alteracoes no HD...\n");
			aux->pageTable[i].M = 0;
		}
	}
	// Atualiza o status do processo
	aux->susp = 1;

	printf(">> Processo %d suspenso <<\n", aux->procNumber);

	return;

}

/* Funcao bin_to_dec --> converte o endereco de binario para decimal

	Entrada --> Endereco de memoria virtual requerido e tamanho em bits do endereco
	Saida --> Endereco em decimal

*/
int bin_to_dec(char *endereco, int tamEnd){

	int dec = 0;
	int i = 0;
	int j = 0;

	for(i = tamEnd - 1; i >= 0; i--){
		dec += (endereco[i] - 48)*pow(2,j);
		j++;
	}

	return dec;

}

/* Funcao mmu --> Funcao que implementa a MMU

	Entrada --> Lista de processos, dados de entrada do arquivo, tamanho do endereco,
a RAM, o tamanho da RAM, contador da qtd de espacos ocupados na RAM, o RELOGIO, o
tamanho da pagina e a fila de instrucao
	Saida --> Nenhuma

*/
void mmu(procList *li, Instruc entrada, int tamEnd, int *RAM, int tamRAM, int *contRAM, Clock *relogio, int tamPage, instrucFila* fila){

	Elem *aux;
	Elem *no_out;
	procRAM *hora;
	int i = 0;
	int end = 0;
	int page = 0;
	int out = 0;
	int achou = 0;

	// Checar se o processo esta na lista
	aux = busca_processo(li, entrada.proc);
	if(aux == NULL){ // Processo nao esta na lista
		printf("\nProcesso %d nao existe...\n", entrada.proc);
		return;
	}

	// Processo na lista

	// Checar se o processo esta suspenso
	if(aux->susp == 1){
		insere_fila_inicio(fila, entrada, 0);
		printf("Processo %d esta suspenso. Instrucao sera adicionada na fila.\n", entrada.proc);
		return;
	}

	// Checar se o endereco que ele quer acessar eh valido
	end = bin_to_dec(entrada.end, tamEnd);
	page = (end/tamPage); // Encontra a pagina que possui o endereco desejado

	if(page > aux->sizeTable - 1){ // Endereco invalido --> aborta o processo e remover da RAM, do RELOGIO e da lista
		printf("\nProcesso tentou acessar area de memoria invalida. Abortando processo...\n");
		// Remover da RAM e do RELOGIO
		for(i = 0; i < aux->sizeTable; i++){
			if(aux->pageTable[i].p_a == 1){
				RAM[aux->pageTable[i].pageFrame] = 0;
				(*contRAM)--;
				remove_relogio(relogio, aux->pageTable[i].pageFrame);

			}
		}
		// Remover da lista
		remove_processo(li, entrada.proc);
		return;
	}


	// Enderecco valido

	// Verificar se a pagina esta na RAM
	if(aux->pageTable[page].p_a == 1){ // Pagina na RAM --> verificar a operacao e modificar R (e tambem M, se for escrita)
		switch(entrada.operation){
			case 'R':
				hora = busca_relogio(relogio, aux->pageTable[page].pageFrame);
				hora->R = 1;
				printf("Endereco ja consta na RAM. Operacao de %c realizada!\n", entrada.operation);
				break;
			case 'W':
				aux->pageTable[page].M = 1;
				hora = busca_relogio(relogio, aux->pageTable[page].pageFrame);
				hora->R = 1;
				printf("Endereco ja consta na RAM. Operacao de %c realizada!\n", entrada.operation);
				break;
		}
		return;
	}


	// Pagina nao esta na RAM --> Page fault
	printf("Page Fault!\n");

	// Verificar se tem espaco livre na RAM
	if((*contRAM) < tamRAM){ // Tem espaco --> adicionar pagina na RAM e no RELOGIO e atualizar tabela de paginas
        printf("Espaco disponivel na RAM. Alocando novo frame!\n");
		for(i = 0; i < tamRAM; i++)
			if(RAM[i] == 0) break;

		RAM[i] = 1;
		(*contRAM)++;
		aux->pageTable[page].pageFrame = i;
		aux->pageTable[page].p_a = 1;
		insere_relogio(relogio, i);
		switch(entrada.operation){
			case 'R':
				hora = busca_relogio(relogio, aux->pageTable[page].pageFrame);
				hora->R = 1;
				break;
			case 'W':
				aux->pageTable[page].M = 1;
				hora = busca_relogio(relogio, aux->pageTable[page].pageFrame);
				hora->R = 1;
				break;
		}
		return;

	}

	printf("Sem espaco na RAM... Aplicando o algoritmo do relogio para substituir uma pagina.\n");

	// Sem espaco livre --> rodar algoritmo do relogio
	out = clock_algorithm(relogio);

	// Identificar processo e pagina que saiu da RAM e atualizar sua tabela de pagina
	no_out = *li;
	while(no_out != NULL && achou == 0){
		for(i = 0; i < no_out->sizeTable; i++)
			if(no_out->pageTable[i].pageFrame == out){
				achou = 1;
				break;
			}
		if(!achou) no_out = no_out->prox;
	}

	printf("Frame a ser substituido: %d (referente a pagina %d do processo %d)\n", no_out->pageTable[i].pageFrame, i, no_out->procNumber);

	no_out->pageTable[i].pageFrame = -1;
	no_out->pageTable[i].p_a = 0;
	if(no_out->pageTable[i].M == 1)
		printf("Pagina que saiu da RAM foi modificada. Salvando alteracoes no HD...\n");
	no_out->pageTable[i].M = 0;


	// Colocar a nova pagina na RAM (o espaco que era ocupado continua 1, pois a nova pagina entrarah lah), no RELOGIO e atualizar sua tabela de pagina
	aux->pageTable[page].pageFrame = out;
	aux->pageTable[page].p_a = 1;
	insere_relogio(relogio, out);
	switch(entrada.operation){
		case 'R':
			hora = busca_relogio(relogio, aux->pageTable[page].pageFrame);
			hora->R = 1;
			break;
		case 'W':
			aux->pageTable[page].M = 1;
			hora = busca_relogio(relogio, aux->pageTable[page].pageFrame);
			hora->R = 1;
			break;
	}

	printf("Frame %d atualizado! Referencia a pagina %d do processo %d.", out, page, aux->procNumber);

	printf("Substituicao realizada com sucesso!\n");

	return;

}

/* Funcao swapper --> Funcao que implementa o Swapper

	Entrada --> Lista de processos, tamanho do processo, numero do processo,
tamanho da RAM, a RAM, contador da qtd de espacos ocupados na RAM e o RELOGIO
	Saida --> Nenhuma

*/
void swapper(procList *li, int tam, int proc, int tamRAM, int *RAM, int *contRAM, Clock *relogio){
    int contador_espaco_livre = (1.25*tamRAM) - (*contRAM - tam); //Contador do espaco que existe disponivel para processos usarem

    Elem *no = *li;

    int *proc_a_retirar = (int *) malloc(1*sizeof(int)); //Vetor que ira conter todos os processos que devem ser retirados
    int j = 0;
    int numProc;

    if(li == NULL)
        return;
    while(contador_espaco_livre < tam && no != NULL){ //Os processos a serem retirados serao primeiramente os processos IO-bound
        if(no->susp == 0){ //I/O-bound = 20%+ instrucoes IO
            if(no->cpuBound == 0 || (no->ioBound/no->cpuBound) > 0.2){
                contador_espaco_livre += no->sizeTable;
                proc_a_retirar = (int *) realloc(proc_a_retirar, (j+1)*sizeof(int));
                proc_a_retirar[j] = no->procNumber;
                j++;
            }
        }
        no = no->prox;
    }

    no = *li; //No eh resetado

    if(contador_espaco_livre < tam){ //Caso os processos IO-bound nao sejam suficientes, retira-se CPU-bounds
        while(contador_espaco_livre < tam && no != NULL){
            if(no->susp == 0 && (no->ioBound/no->cpuBound) <= 0.2){
                contador_espaco_livre += no->sizeTable;
                proc_a_retirar = (int *) realloc(proc_a_retirar, (j+1)*sizeof(int));
                proc_a_retirar[j] = no->procNumber;
                j++;
            }
        }
    }

    numProc = j;

    for(j=0; j<numProc; j++){ //Suspensao dos processos
        suspender_processo(proc_a_retirar[j], li, RAM, relogio, contRAM);
    }

}


/* Funcao alocar_espaco --> Aloca espaco na RAM e no HD quando um processo eh criado

	Entrada --> Lista de processos, tamanho do processo, numero do processo,
tamanho da RAM, a RAM, contador da qtd de espacos ocupados na RAM, o RELOGIO e o
tamamho do HD
	Saida --> Nenhuma

*/
void alocar_espaco(procList *li, int tam, int proc, int tamRAM, int *RAM, int *contRAM, Clock *relogio, int tamHD){

    Elem *no = *li;
	int contHD = 0; //Essa variavel sera utilizada para verificar se todos os processos irao caber na memoria secundaria.
	int i, j = 0; //Contadores

    printf("\n");

    if(tam > 1.25*tamRAM){ //Verifica se o processo excede o tamanho permitido.
        printf("Processo %d eh maior que o tamanho permitido e portanto nao sera inicializado.\n", proc);
        return;
    }

    while(no != NULL){ //Soma do tamanho de todos os processos com o novo.
        contHD += no->sizeTable;
        no = no->prox;
    }
    if(contHD > tamHD){ //Verifica se todos os processos irao caber na memoria secundaria.
    	printf("Nao ha espaco para alocar espaco para o Processo %d na memoria secundaria, ele nao sera inicializado.\n", proc);
    	return;
    }

    *contRAM = tam; //contRAM sera utilizado aqui para verificar se os processos nao suspensos cabem em 1.25*capacidade da memoria RAM
    no = *li; //Ponteiro resetado

    while(no != NULL){ //Soma do tamanho de todos os processos nao suspensos com o novo processo.
        if(no->susp == 0){
            *contRAM += no->sizeTable;
        }
        no = no->prox;
    }
    if(*contRAM > 1.25 * tamRAM){ //Se exceder, efetuar swap.
        printf("Espaco de memoria permitido por processos ativos excedido. Ativando swapper...\n");
        swapper(li, tam, proc, tamRAM, RAM, contRAM, relogio);
    }

    no = busca_processo(li, proc);
    if(no){ //Se o processo ja existe, ele sera retirado da suspensao.
        no->susp = 0;
    } else{ //Caso contrario, eh adicionado a lista
        no = insere_processo(li, proc, tam);
    }

    no->pageTable = (Table *) calloc(tam, sizeof(Table)); //Alocacao da tabela de paginas
    for(i = 0; i < tamRAM && j < tam; i++){ //Alocacao da RAM para o processo.
        if(!RAM[i]){
            printf("Pagina %d da RAM foi alocada para a pagina %d da tabela e foi inserida no relogio.\n", i, j);
            RAM[i] = 1;
            no->pageTable[j].M = 0;
            no->pageTable[j].pageFrame = i;
            no->pageTable[j].p_a = 1;
            insere_relogio(relogio, i);
            j++;
        }
    }
    if(j < tam){ //Se a RAM nao for o suficiente, as outras paginas estarao no HD
        for(; j < tam; j++){
            printf("Nao ha espaco na RAM para a pagina %d da tabela.\n", j);
            no->pageTable[j].M = 0;
            no->pageTable[j].pageFrame = -1;
            no->pageTable[j].p_a = 0;
        }
    }

}

/* Funcao verifica_susp --> Verifica se ha processos suspensos

	Entrada --> Lista de processos
	Saida --> Retorna 1 se ha processos suspenso. Caso contrario, retona 0.

*/
int verifica_susp(procList *li){

    Elem *no;
    no = *li;

    if(li == NULL)
        return 0;
    while(no != NULL && no->susp == 0){
        no = no->prox;
    }

    if(no == NULL)
        return 0;
    else{
        return 1;
    }
}

