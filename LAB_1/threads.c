#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ll long long int // abreviando para facilitar a escrita
#define SIZE 10000 // tamanho do array
#define NTHREADS 2 // numero de threads a serem criadas

// estrutura de parâmetros a serem passados para a função
typedef struct {
    int id; // id a thread
    ll *v; // ponteiro para o primeiro elemento do vetor

    // primeiro e último elementos a serem modificados pela thread
    int first;
    int last;
} parameters;

// função a ser executada como uma thread
void *pow_elements(void *args) {
    parameters *p = (parameters*)args; // instanciando os parâmetros recebidos

    for(int i=p->first; i<p->last; i++)
        // elevando o elemento de índice i no array ao quadrado
        *((p->v)+i) *= *((p->v)+i);

    pthread_exit(NULL);
}

int main() {
    ll v[SIZE]; // array a ser usado
    ll original_v[SIZE]; // array auxiliar para verificação de corretude
    pthread_t threads_id[NTHREADS];
    parameters *args;
    int first = 0;
    int last = SIZE/NTHREADS; // a primeira thread terá a mediana do array como último elemento a ser modificado

    /* povoando o array com números para o programa ser testado.
       utilizando associação direta (índice 10 guarda o elemento 10, etc)
    */
    for(int i=0; i<SIZE; i++) {
        v[i] = i;
        original_v[i] = i;
    }

    // instanciando as threads
    for(int i=0; i<NTHREADS; i++) {
        args = malloc(sizeof(parameters)); // alocando memória para os parâmetros
        args->id = i;
        args->v = &v[0]; // passando a referência do primeiro elemento do array v
        args->first = first;
        args->last = last;
        
        // criando a thread de índice i, passando os parâmetros args
        pthread_create(&threads_id[i], NULL, pow_elements, (void*)args);
        printf("Thread %d criada\n", i);

        // atualizando os valores de first e last a serem considerados pela próxima thread que será criada
        first = last;
        last = SIZE;
    }

    // esperando o término das threads criadas
    for(int i=0; i<NTHREADS; i++)
        pthread_join(threads_id[i], NULL);

    // verificando a corretude da execução
    for(int i=0; i<SIZE; i++) {
        // caso um elemento no array original_v seja diferente do seu quadrado (armazenado em v)
        // imprime qual a posição do erro e termina com erro
        if(original_v[i]*original_v[i] != v[i]) {
            printf("\nErro em %d: antes = %lld, depois = %lld", i, original_v[i], v[i]);
            exit(-1);
        }
    }
    
    // caso tudo tenha corrido como o esperado
    printf("Todos os elementos do array foram elevados ao quadrado!\n");

    pthread_exit(NULL);
}