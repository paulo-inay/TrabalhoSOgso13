#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define MAX_BUFFER 80 //Tamanho do buffer.
#define MAX_PRODUCE 200 //Numero de itens produzidor pelo produtor.

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; //Inicializacao das variaveis condicionais e o mutex.
pthread_cond_t cond_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_p = PTHREAD_COND_INITIALIZER;

int *buffer; 
int *ini , *fim; //Ponteiros: ini aponta para ultimo espaco livre, fim aponta para ultimo espaco ocupado.
int quant = 0; //Quantidade de espacos ocupados.

void *produtor(void *ptr){
        for(int i = 0; i < MAX_PRODUCE; i++){
                pthread_mutex_lock(&mtx); 
                while(quant == MAX_BUFFER) pthread_cond_wait(&cond_p, &mtx);

                //Inicio Regiao Critica

                fim = buffer + ((fim + 1 - buffer) % MAX_BUFFER);
                *fim = 1;
                quant++;

                printf("Buffer: "); //Comente as proximas 3 linhas e descomente as linhas a seguir para mudar o print.
                for(int j = 0; j < MAX_BUFFER; j++) printf("%d ",buffer[j]);
                printf("\n");
                //printf("Espacos vazios: %d / Ponteiro de inicio: %ld\n", MAX_BUFFER - quant, ini - buffer);
                //printf("Espacos ocupados: %d / Ponteiro de fim: %ld\n", quant, fim - buffer);

                //Fim Regiao Critica

                pthread_cond_signal(&cond_c);
                pthread_mutex_unlock(&mtx);
        }
        pthread_exit(0);
}

void *consumidor(void *ptr){
        int valor;

        for(int i = 0; i < MAX_PRODUCE; i++){
                pthread_mutex_lock(&mtx);
                while(quant == 0) pthread_cond_wait(&cond_c, &mtx);

                //Inicio Regiao Critica

                ini = buffer + ((ini + 1 - buffer) % MAX_BUFFER);
                valor = *ini;
                *ini = 0;
                quant--;

                printf("Buffer: "); //Comente as proximas 3 linhas e descomente as linhas a seguir para mudar o print.
                for(int j = 0; j < MAX_BUFFER; j++) printf("%d ",buffer[j]);
                printf("\n");
                //printf("Espacos vazios: %d / Ponteiro de inicio: %ld\n", MAX_BUFFER - quant, ini - buffer);
                //printf("Espacos ocupados: %d / Ponteiro de fim: %ld\n", quant, fim - buffer);

                //Fim Regiao Critica

                pthread_cond_signal(&cond_p);
                pthread_mutex_unlock(&mtx);
        }
        pthread_exit(0);
}

int main(){
        pthread_t t_produtor, t_consumidor;
        int errp, errc;

        buffer = calloc(MAX_BUFFER, sizeof(int));
        if(!buffer){
                printf("Erro na alocacao de memoria\n");
                exit(1);
        }

        ini = buffer + MAX_BUFFER - 1; //Inicializado para o ultimo espaco do buffer, pois as funcoes produtor e consumidor
        fim = buffer + MAX_BUFFER - 1; //"incrementam" o ponteiro antes de modificar valores no buffer.

        errc = pthread_create(&t_consumidor, 0, consumidor, 0);
        if(errc){
                printf("Erro na criacao da thread consumidor\n");
                exit(errc);
        }

        errp = pthread_create(&t_produtor, 0, produtor, 0);
        if(errp){
                printf("Erro na criacao da thread produtor\n");
                exit(errp);
        }

        pthread_join( t_consumidor, NULL); //Espera o termino das threads.
        pthread_join( t_produtor, NULL);

        return 0;
}
