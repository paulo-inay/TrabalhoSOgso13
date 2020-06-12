// Sistemas Operacionais - Trabalho 3 - Gerenciamento de memoria
//
// Joao Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "memGen.h"

/* * * * * * * * * * Funcoes para manejo das estruturas * * * * * * * * * */

/* Funcoes para a fila */

/* Funcao libera_fila --> Aloca um espaco para a fila

	Entrada --> Nenhuma
	Saida --> Ponteiro para o espaco alocado para a fila

*/
instrucFila* cria_fila(){
	instrucFila* li;

    li = (instrucFila*) malloc(sizeof(instrucFila));
    if(li != NULL)
        *li = NULL;
    return li;
}

/* Funcao libera_fila --> Libera o espaco alocado para a fila

	Entrada --> Fila
	Saida --> Nenhuma

*/
void libera_fila(instrucFila* li){

	if(li != NULL){
		Instruc* no;

        while((*li) != NULL){
            no = *li;
            *li = (*li)->prox;
            if(no->end) free(no->end);
            free(no);
        }

        free(li);

    }

}

/* Funcao insere_fila_inicio --> Insere um elemento no inicio da fila

	Entrada --> Fila, instrucao a ser inserida e tamanho do processo
	Saida --> Nenhuma

*/
void insere_fila_inicio(instrucFila* li, Instruc instrucao, int tamanho){

    Instruc* no;

    if (li == NULL){
    	printf("Erro na insercao. Fila nao encontrada...\n");
		exit(1);
	}

    no = (Instruc*) malloc(sizeof(Instruc));

    if (no == NULL){
    	printf("Erro na insercao. Falha ao alocar espaco para elemento da fila...\n");
		exit(1);
	}

    no->proc = instrucao.proc;
    no->operation = instrucao.operation;
    no->tam = tamanho;
    no->end = strdup(instrucao.end);
    no->prox = (*li);
    if(no->prox)
        no->prox->ant = no;
    no->ant = NULL;
    *li = no;

    return;
}

/* Funcao busca_fila_final --> Busca um elemento no final da fila

	Entrada --> Fila
	Saida --> Elemento buscado

*/
Instruc* busca_fila_final(instrucFila* li){

	Instruc *no;
    no = *li;

    if(li == NULL){
    	printf("Erro na busca. Fila nao encontrada...\n");
		exit(1);
	}
    if(no == NULL)
        return NULL;
    while(no->prox != NULL){
        no = no->prox;
    }
    return no;

}

/* Funcao remove_fila_final --> Remove um elemento do final da fila

	Entrada --> Fila
	Saida --> 1, em caso de sucesso na remocao; 0, caso contrario

*/
int remove_fila_final(instrucFila* li){

    if(li == NULL)
        return 0;
    if((*li) == NULL) //lista vazia
        return 0;

    Instruc *ant, *no = *li;

    while(no->prox != NULL){
        ant = no;
        no = no->prox;
    }

    if(no == (*li)) //remover o primeiro?
        *li = no->prox;
    else
        ant->prox = no->prox;
    if (no->end) free(no->end);
    free(no);
    return 1;
}

/* Funcao criar_fila_proc --> Cria uma nova fila com as instrucoes de um processo que voltou depois de ser suspenso

	Entrada --> Fila de suspensao, fila do processo e numero do processo
	Saida --> Nenhuma

*/
void criar_fila_proc(instrucFila *fila_susp, instrucFila *fila_proc, int numProc){
    Instruc *no = busca_fila_final(fila_susp);
    Instruc *aux = no;
    if(fila_susp == NULL || fila_proc == NULL){
    	printf("Erro na criacao da fila. Fila nao encontrada...\n");
		exit(1);
	}
    if(no == NULL){
    	printf("Erro na criacao da fial. Elemento nao encontrado...\n");
		exit(1);
	}

    while(no != NULL){
        if(no->proc == numProc){
            insere_fila_inicio(fila_proc, *no, 0);
            aux = no;
            no = no->ant;
            if(no)
                no->prox = aux->prox;
            else
                *fila_susp = aux->prox;
            if(aux->end)
                free(aux->end);
            free(aux);
        } else no = no->ant;
    }

    return;
}


/* Funcoes para a lista encadeada */

/* Funcao cria_lista --> Aloca um espaco para a lista

	Entrada --> Nenhuma
	Saida --> Ponteiro para o espaco alocado para a lista

*/
procList* cria_lista(){
	procList* li;

    li = (procList*) malloc(sizeof(procList));
    if(li != NULL)
        *li = NULL;
    return li;
}

/* Funcao libera_lista --> Libera o espaco alocado para a lista

	Entrada --> Lista de processos
	Saida --> Nenhuma

*/
void libera_lista(procList* li){

	if(li != NULL){
		Elem* no;

        while((*li) != NULL){
            no = *li;
            *li = (*li)->prox;
            free(no->pageTable);
            free(no);
        }

        free(li);

    }

}

/* Funcao insere_processo --> Insere um processo novo na lista

	Entrada --> Lista de processos, numero do processo e tamanho do processo
	Saida --> Processo criado

*/
Elem* insere_processo(procList* li, int processo, int tamanho){

	if (li == NULL){
		printf("Erro na insercao. Lista de processos nao encontrada...\n");
		exit(1);
	}

    Elem *no = (Elem*) malloc(sizeof(Elem));

    if (no == NULL){
    	printf("Erro na insercao. Falha ao alocar espaco para o elemento do Lista...\n");
		exit(1);
	}

    no->procNumber = processo;
    no->pageTable = (Table*) calloc(tamanho, sizeof(Table));
    no->sizeTable = tamanho;
    no->susp = 0;
    no->cpuBound = 0;
    no->ioBound = 0;

    if ((*li) == NULL) { //lista vazia: insere in�cio
        no->prox = NULL;
        *li = no;
        return no;
    }
    else {

		Elem *ant, *atual = *li;

        while (atual != NULL && atual->procNumber < processo) {
            ant = atual;
            atual = atual->prox;
        }

        if (atual == *li) { //insere in�cio
            no->prox = (*li);
            *li = no;
        }else{
            no->prox = atual;
            ant->prox = no;
        }

        return no;

	}

}

/* Funcao busca_processo --> Busca na lista um processo de interesse

	Entrada --> Lista de processos e numero do processo a ser buscado
	Saida --> O processo solicitado

*/
Elem* busca_processo(procList* li, int processo){

	Elem *no;
    no = *li;

    if(li == NULL){
    	printf("Erro na busca. Lista de processos nao encontrada...\n");
		exit(1);
	}
    while(no != NULL && no->procNumber != processo){
        no = no->prox;
    }

    if(no == NULL)
        return NULL;
    else{
        return no;
    }

}

/* Funcao remove_processo --> Remove um processo da lista

	Entrada --> Lista de processos e numero do processo a ser removido
	Saida --> Nenhuma

*/
void remove_processo(procList* li, int processo){

	if(li == NULL){
		printf("Erro na remocao. Lista de processos nao encontrada...\n");
		exit(1);
	}
    if((*li) == NULL){ // Lista vazia
        printf("Lista de processos esta vazia...\n");
		return;
    }

	Elem *ant, *no = *li;

	while(no != NULL && no->procNumber != processo){
        ant = no;
        no = no->prox;
    }

    if(no == *li) //remover o primeiro?
        *li = no->prox;
    else
        ant->prox = no->prox;

    free(no->pageTable);
    free(no);

    return;
}

/* Funcao printa_processo --> Imprime a lista de processos

	Entrada --> Lista de processos
	Saida --> Nenhuma

*/
void printa_processo(procList *li){

	int i = 0;

    Elem *no = *li;
    while(no){
        printf("\nNumero do processo: %d, Suspenso: %d\n", no->procNumber, no->susp);
        printf("Tabela de Paginas:\n");
        for(i = 0; i < no->sizeTable; i++){
            printf("    Bit Presente/Ausente: %d, Bit Modificado: %d, Page Frame: %d\n", no->pageTable[i].p_a, no->pageTable[i].M, no->pageTable[i].pageFrame);
        }
        no = no->prox;
    }

}


/* Funcoes para a lista circular */

/* Funcao cria_relogio --> Aloca um espaco para o RELOGIO

	Entrada --> Nenhuma
	Saida --> Ponteiro para o espaco alocado para o RELOGIO

*/
Clock* cria_relogio(){

	Clock* li = (Clock*) malloc(sizeof(Clock));

	if(li != NULL)
        *li = NULL;
    return li;

}

/* Funcao libera_relogio --> Libera todo o espaco alocado para o RELOGIO

	Entrada --> o RELOGIO
	Saida --> Nenhuma

*/
void libera_relogio(Clock* li){

	if(li != NULL && (*li) != NULL){

	    procRAM *aux, *no = *li;
        while((*li) != no->prox){
            aux = no;
            no = no->prox;
            free(aux);
        }

        free(no);
        free(li);

    }

}

/* Funcao insere_relogio --> insere um novo elemento no relogio

	Entrada --> o RELOGIO e o numero da moldura ppara o novo elemento
	Saida --> Nenhuma

*/
void insere_relogio(Clock* li, int moldura){

	if(li == NULL){
		printf("Erro na insercao. RELOGIO nao encontrado...\n");
		exit(1);
	}

	procRAM *no = (procRAM*) malloc(sizeof(procRAM));

	if(no == NULL){
		printf("Erro na insercao. Falha ao alocar espaco para o elemento do RELOGIO...\n");
		exit(1);
	}

    no->framePage = moldura;
    no->R = 0;

    if((*li) == NULL){ //lista vazia: insere in�cio
        *li = no;
        no->prox = no;
    }else{
        procRAM *aux = *li;
        while(aux->prox != (*li)){
            aux = aux->prox;
        }
        aux->prox = no;
        no->prox = *li;
    }

    return;
}

/* Funcao busca_relogio --> Busca um elemento no relogio de acordo
com o numero do frame desejado

	Entrada --> o RELOGIO e a moldura a ser buscada
	Saida --> o elemento desejado ou NULL se ele nao existe

*/
procRAM* busca_relogio(Clock* li, int moldura){

	procRAM *no;
    no = *li;

    if(li == NULL){
    	printf("Erro na busca. RELOGIO nao encontrado...\n");
		exit(1);
	}
    while(no->prox != (*li) && no->framePage != moldura){
        no = no->prox;
    }

    return no;

}

/* Funcao remove_relogio --> Remove uma moldura da lista circular
que representa o relogio

	Entrada --> o RELOGIO e a moldura a ser retirada
	Saida --> Nenhuma

*/
void remove_relogio(Clock* li, int moldura){

	if(li == NULL){
		printf("Erro na remocao. RELOGIO nao encontrado...\n");
		exit(1);
	}

    if((*li) == NULL){ // RELOGIO vazio
        printf("RELOGIO vazio...\n");
		return;
	}

    procRAM *no = *li;

    if(no->framePage == moldura){ // Remover do in�cio
        if(no == no->prox){ // RELOGIO fica vazio
            free(no);
            *li = NULL;
            return;
        }else{
            procRAM *ult = *li;
            while(ult->prox != (*li)) // Procura o �ltimo
                ult = ult->prox;
            ult->prox = (*li)->prox;
            *li = (*li)->prox;
            free(no);
            return;
        }
    }

    procRAM *ant = no;
    no = no->prox;

    while(no != (*li) && no->framePage != moldura){
        ant = no;
        no = no->prox;
    }

    ant->prox = no->prox;
    free(no);

    return;
}


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

