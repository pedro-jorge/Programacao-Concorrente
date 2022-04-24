#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define matrix float** // definindo o tipo "matrix" como um ponteiro duplo
#define MAX 9999.0 // número máximo a ser gerado em um elemento das matrizes aleatórias, pode ser alterado

// estrutura para os parâmatros a serem recebidos nas threads
typedef struct {
    int id; // id da thread
    int n; // dimensão da matriz
    int first_line, last_line; // primeira e última linhas a serem consideradas pela thread na multiplicação
    matrix m1; 
    matrix m2;
    matrix result; // ponteiro para a matriz resultado a ser modificada pela thread
} parameters;

// gerador de números aleatórios para os elementos das matrizes
float random_real_number(float max) {
    return (float)rand()/(float)(RAND_MAX/max);
}

// cria e retorna uma matriz NxN sem inicializações de valores
matrix create_matrix(int n) {
    float** m = (float**)malloc(n * sizeof(float*));
    for(unsigned i=0; i<n; i++)
        m[i] = (float*)malloc(n*sizeof(float));
    
    return m;
}

// cria e retorna uma matriz NxN inicializada com valores aleatórios
matrix create_random_matrix(int n) {
    float** m = (float**)malloc(n * sizeof(float*));
    for(unsigned i=0; i<n; i++)
        m[i] = (float*)malloc(n*sizeof(float));
    
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            m[i][j] = random_real_number(MAX);
        }
    }

    return m;
}

// realiza a multiplicação das matrizes m1 e m2 de forma sequencial
matrix sequential_multiplication(matrix m1, matrix m2, int n) {
    matrix result = create_matrix(n);

    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            float ij = 0;
            for(int k=0; k<n; k++)
                ij += m1[i][k] * m2[k][j];
            
            result[i][j] = ij;
        }
    }

    return result;
}

// função a ser executada pelas threads para a multiplicação
void* concurrent_multiplication(void *args) {
    parameters *p = (parameters*)args;
    
    // considera a multiplicação somente da linha first_line até a linha last_line
    for(int i=p->first_line; i<p->last_line; i++) {
        for(int j=0; j<p->n; j++) {
            float ij = 0;
            for(int k=0; k<p->n; k++)
                ij += p->m1[i][k] * p->m2[k][j];
            p->result[i][j] = ij;
        }
    }

    pthread_exit(NULL);
}

// função para imprimir matrizes, caso seja necessário
void print_matrix(matrix m, int n) {
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++)
            printf("%f ", m[i][j]);
        printf("\n");
    }
}

// função booleana que verifica a igualdade entre as matrizes m1 e m2
int compare_matrices(matrix m1, matrix m2, int n) {
    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            if(m1[i][j] != m2[i][j]) return 0;
    return 1;
}

int main(int argc, char **argv) {
    // armazenamento dos parâmetros 
    int n = atoi(argv[1]); // dimensão das matrizes
    int nthreads = atoi(argv[2]); // número de threads desejadas
    
    double s_start, s_end, s_delta; // valores para o timer da multiplicação sequencial
    double c_start, c_end, c_delta; // valores para o timer da multiplicação concorrente

    // inicialização das matrizes
    matrix m1 = create_random_matrix(n);
    matrix m2 = create_random_matrix(n);
    matrix result = create_matrix(n);

    // executando e medindo o tempo da execução
    // sequencial
    GET_TIME(s_start);
    matrix s_result = sequential_multiplication(m1, m2, n);
    GET_TIME(s_end);
    s_delta = s_end - s_start;

    // executando e medindo o tempo da execução
    // concorrente
    GET_TIME(c_start);

    pthread_t threads_id[nthreads];
    parameters* args;
    int first = 0;
    int last = n / nthreads;

    for(int i=0; i<nthreads; i++) {
        args = malloc(sizeof(parameters));
        args->id = i;
        args->n = n;
        args->m1 = m1;
        args->m2 = m2;
        args->first_line = first;
        args->last_line = last;
        args->result = result;

        pthread_create(&threads_id[i], NULL, concurrent_multiplication, (void*)args);
        // printf("Thread %d criada\n", i);

        first = last;
        last = (last+(n/nthreads) <= n ? last+(n/nthreads) : n);
    }

    for(int i=0; i<nthreads; i++)
        pthread_join(threads_id[i], NULL);

    GET_TIME(c_end);
    c_delta = c_end - c_start;

    // verificando a corretude
    /* retorna 1 caso ambos os resultados sejam iguais
       0 caso contrário
    */
    printf("%d\n", compare_matrices(s_result, result, n));

    // medindo o tempo de cada uma das abordagens
    printf("Tempo sequencial: %lf\n", s_delta);
    printf("Tempo concorrente: %lf\n", c_delta);

    if(s_delta > c_delta) {
        double ganho = s_delta / c_delta;
        printf("Ganho obtido: %lf", ganho);
    }
    else printf("Sem ganhos.");

    return 0;
}