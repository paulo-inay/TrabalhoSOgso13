// Sistemas Operacionais - Trabalho 3 - Gerenciamento de memoria
//
// João Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691

#include <stdio.h>
#include <stdlib.h>


// Definicao dos dados do arquivo de entrada
struct dados{
	int proc;						// Numero do processo
	char operation;					// Tipo de operacao a fazer
	int tam;						// Tamanho do processo
	char *end;						// Endereco para operacao
};

typedef struct dados Instruc;
typedef struct dados *Fila;			// Fila de processos suspensos

/*************************************************************************************/

// Definicao da tabela de paginas
struct elemTable{
	int M;							// Bit de modificacao
	int p_a;						// Bit presente/ausente
	int pageFrame;					// Numero da moldura de pagina
};

typedef struct elemTable Table;

/*************************************************************************************/

// Definicao da lista de processos
struct process{
    int procNumber;					// Numero do processo
	Table *pageTable;				// Tabela de paginas
	int sizeTable;					// Tamanho da tabela
	int cpuBound;					// Qtd de instrucoes io bound
	int ioBound;					// Qtd de instrucoes cpu bound
	int susp;						// Indica se o processo esta suspenso ou nao
    struct process *prox;			// Ponteiro para o proximo processo da lista
};

typedef struct process Elem;
typedef struct process* procList;	// Lista de processos

// Funcoes para a lista de processos

procList* cria_lista(){
	Lista* li;

    li = (procList*) malloc(sizeof(procList));
    if(li != NULL)
        *li = NULL;
    return li;
}

void libera_lista(procList* li){
	
	if(li != NULL){
		Elem* no;
		
        while((*li) != NULL){
            no = *li;
            *li = (*li)->prox;
            free(no);
        }
        
        free(li);
        
    }
    
}

void insere_processo(procList* li, int processo, int tamanho){ // Ordenado???
    
	if (li == NULL) return ERRO;
    Elem *no = (Elem*) malloc(sizeof(Elem));
    if (no == NULL) return ERRO;

    no->procNumber = processo;
    no->pageTable = (elemTable*)calloc(tamanho, sizeof(elemTable));
    no->sizeTable = tamanho;
    no->susp = 0;
    no->cpuBound = 0;
    no->ioBound = 0;
    
    if ((*li) == NULL) { //lista vazia: insere início
        no->prox = NULL;
        *li = no;
        return;
    }
    else {
        
		Elem *ant, *atual = *li;
        
        while (atual != NULL && atual->procNumber < processo) {
            ant = atual;
            atual = atual->prox;
        }
        
        if (atual == *li) { //insere início
            no->prox = (*li);
            *li = no;
        }else{
            no->prox = atual;
            ant->prox = no;
        }
        
        return;
        
	}
	
}

Elem* busca_processo(procList* li, int processo){
	
	Elem *no;
    no = *li;
    
    if(li == NULL)
        return ERRO;
    while(no != NULL && no->procNumber != processo){
        no = no->prox;
    }
    
    if(no == NULL)
        return NULL;
    else{
        return no;
    }
    
}

void remove_processo(procList* li, int processo){
	
	if(li == NULL)
        return ERRO;
    if((*li) == NULL)//lista vazia
        return ERRO;
    
	Elem *ant, *no = *li;
    
	while(no != NULL && no->procNumber != processo){
        ant = no;
        no = no->prox;
    }

    if(no == *li) //remover o primeiro?
        *li = no->prox;
    else
        ant->prox = no->prox;
    free(no);
    
    return;
}

/*************************************************************************************/

// Definicao da lista circular (Relogio)
struct hour{
	int framePage;					// Moldura do processo
	int R;							// Bit de referenciado
	struct hour *prox;				// Ponteiro para o proximo processo da lista
};

typedef struct hour procRAM;
typedef struct hour* Clock;			// Lista circular de processos na RAM

// Funcoes para a lista circular

Clock* cria_relogio(){
	
	Lista* li = (Clock*) malloc(sizeof(Clock));
    
	if(li != NULL)
        *li = NULL;
    return li;
    
}

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

int insere_relogio(Clock* li, int moldura){
	
	if(li == NULL)
        return 0;
    
	procRAM *no = (procRAM*) malloc(sizeof(procRAM));
    
	if(no == NULL)
        return 0;
        
    no->framePage = moldura;
    no->R = 0;
    
    if((*li) == NULL){ //lista vazia: insere início
        *li = no;
        no->prox = no;
    }else{
        Elem *aux = *li;
        while(aux->prox != (*li)){
            aux = aux->prox;
        }
        aux->prox = no;
        no->prox = *li;
    }
    
    return 1;
}

int remove_relogio(Clock* li, int moldura){
    
	if(li == NULL)
        return 0;
        
    if((*li) == NULL)//lista vazia
        return 0;
        
    procRAM *no = *li;
    
    if(no->framePage == moldura){ //remover do início
        if(no == no->prox){ //lista fica vazia
            free(no);
            *li = NULL;
            return 1;
        }else{
            Elem *ult = *li;
            while(ult->prox != (*li))//procura o último
                ult = ult->prox;
            ult->prox = (*li)->prox;
            *li = (*li)->prox;
            free(no);
            return 1;
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
    
    return 1;
}

/*************************************************************************************/

void mmu(){
	
	procRam *ponteiro;						// Ponteiro do algoritmo do relogio
	
}

/*************************************************************************************/

void swapper(){
	
}

/*************************************************************************************/

void alocar_espaco(){
	
}

/*************************************************************************************/

void suspender_processo(){
	
}

/*************************************************************************************/


int main(){
	
	Instruc entrada;						// Dados de entrada
	Elem *processo;
	int *RAM;								// Memoria principal
	int tamPage = 4;						// Tamanho das paginas e das molduras
	int tamEnd = 8;							// Tamanho em bits do endereco logico
	int tamRAM = 32;						// Tamanho da RAM
	int contRAM = 0;						// Contador para qtd de espacos ocupados na RAM
	int tamHD = 64;							// Tamanho maximo da memoria secundaria
	FILE *fp;
	
	RAM = (int*)calloc(tamRAM, sizeof(int));
	
	fp = fopen("arquivo.txt", "r");
	if(fp == NULL){
		printf("Erro na abertura do aquivo...");
		exit(1);
	}
	
	fscanf(fp, "%d %c", entrada.proc, entrada.operation);
	
	while(entrada.proc != 0){
		
		switch(entrada.operation){
			case 'C':
				alocar_espaco();
				break;
			case 'R':
				entrada.end = (char*)calloc(tamEnd,sizeof(char));
				fscanf(fp, "%s", entrada.end);
				mmu();
				break;
			case 'W':
				entrada.end = (char*)calloc(tamEnd,sizeof(char));
				fscanf(fp, "%s", entrada.end);
				mmu();
				break;
			case 'P':
				processo = busca_processo();
				processo->cpuBound++;
				break;
			case 'I':
				processo = busca_processo();
				processo->ioBound++;
				break;		
		}
		
		fscanf(fp, "%d %c", entrada.proc, entrada.operation);
	}
	
	fclose(fp);
	
	return 0;
	
}
