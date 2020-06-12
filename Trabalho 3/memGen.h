// Sistemas Operacionais - Trabalho 3 - Gerenciamento de memoria
//
// Joao Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691
//
// Biblioteca para o simulador de gerenciamento de mem�ria virtual



/* * * * * * * * * * * * * * * Estruturas * * * * * * * * * * * * * * */

/* Definicao dos dados do arquivo de entrada */
struct dados{
	int proc;						// Numero do processo
	char operation;					// Tipo de operacao a fazer
	int tam;						// Tamanho do processo
	char *end;						// Endereco para operacao
	struct dados *prox;				// Ponteiro para o proximo dado da fila
	struct dados *ant;				// Ponteiro para o dado anterior da fila
};

typedef struct dados Instruc;
typedef struct dados *instrucFila;  // Fila de processos suspensos


/* Definicao da tabela de paginas */
struct elemTable{
	int M;							// Bit modificada (bit M)
	int p_a;						// Bit presente/ausente
	int pageFrame;					// Numero da moldura de pagina
};

typedef struct elemTable Table;


/* Definicao dos processos */
struct process{
    int procNumber;					// Numero do processo
	Table *pageTable;				// Tabela de paginas
	int sizeTable;					// Tamanho da tabela
	int cpuBound;					// Qtd de instrucoes cpu-bound
	int ioBound;					// Qtd de instrucoes io-bound
	int susp;						// Indica se o processo esta suspenso ou nao
    struct process *prox;			// Ponteiro para o proximo processo da lista
};

typedef struct process Elem;
typedef struct process* procList;	// Lista de processos


/* Definicao da lista circular (Relogio) */
struct hour{
	int framePage;					// Moldura do processo
	int R;							// Bit de referenciado
	struct hour *prox;				// Ponteiro para o proximo processo da lista
};

typedef struct hour procRAM;
typedef struct hour* Clock;			// Lista circular de processos na RAM



/* * * * * * * * * * Funcoes para manejo das estruturas * * * * * * * * * */

/* Funcoes para a fila */
instrucFila* cria_fila();
void libera_fila(instrucFila* li);
void insere_fila_inicio(instrucFila* li, Instruc instrucao, int tamanho);
Instruc* busca_fila_final(instrucFila* li);
int remove_fila_final(instrucFila* li);
void criar_fila_proc(instrucFila *fila_susp, instrucFila *fila_proc, int numProc);

/* Funcoes para a lista encadeada */
procList* cria_lista();
void libera_lista(procList* li);
Elem* insere_processo(procList* li, int processo, int tamanho);
Elem* busca_processo(procList* li, int processo);
void remove_processo(procList* li, int processo);
void printa_processo(procList *li);

/* Funcoes para a lista circular */
Clock* cria_relogio();
void libera_relogio(Clock* li);
void insere_relogio(Clock* li, int moldura);
procRAM* busca_relogio(Clock* li, int moldura);
void remove_relogio(Clock* li, int moldura);



/* * * * * * * * * * Funcoes proprias do simulador * * * * * * * * * */

/* Funcao para identificar a pagina que sera substituida usando o Algoritmo do Rel�gio */
int clock_algorithm(Clock *li);

/* Funcao para retirar da RAM e do RELOGIO as paginas de um processo que sera suspenso */
void suspender_processo(int processo, procList *li, int *RAM, Clock *relogio, int *contRAM);

/* Funcao que converte o endereco de binario para decimal */
int bin_to_dec(char *endereco, int tamEnd);

/* Funcao que implementa a MMU */
void mmu(procList *li, Instruc entrada, int tamEnd, int *RAM, int tamRAM, int *contRAM, Clock *relogio, int tamPage, instrucFila* fila);

/* Funcao que implementa o Swapper */
void swapper(procList *li, int tam, int proc, int tamRAM, int *RAM, int *contRAM, Clock *relogio);

/* Funcao que aloca espaco quando um processo eh criado */
void alocar_espaco(procList *li, int tam, int proc, int tamRAM, int *RAM, int *contRAM, Clock *relogio, int tamHD);

/* Funcao para verificar se ha processos suspensos */
int verifica_susp(procList *li);

