// Sistemas Operacionais - Trabalho 3 - Gerenciamento de memoria
//
// Joao Marcos Della Torre Divino - NUSP 10377708
// Paulo Inay Cruz - NUSP 10388691
//
// Biblioteca para o simulador de gerenciamento de memoria virtual

#include "estDados.h"


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

