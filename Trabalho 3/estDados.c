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