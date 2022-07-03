#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include "timer.h"

// definindo ull para facilitar a escrita e leitura
#define ull unsigned long long int
// tamanho máximo de um inteiro do vetor
#define MAX 99999

// tamanho do vetor
const ull SIZE =  1000000;
// número de threads
const ull NTHREADS = 2;

// estrutura a ser passada como parâmetro
// para as threads
typedef struct {
    ull* arr;
    ull thread_id;
    ull size;
} parameters;

// merge clássico
void merge(ull arr[], ull left, ull mid, ull right) {
    ull i, j, k;
    ull l = mid - left + 1;
    ull r = right - mid;
    ull* L = (ull*)malloc(sizeof(ull) * l);
    ull* R = (ull*)malloc(sizeof(ull) * r);

    for(i=0; i<l; i++)
        L[i] = arr[left+i];
    for(j=0; j<r; j++)
        R[j] = arr[mid+1+j];

    i = 0;
    j = 0;
    k = 0;
    while(i<l && j<r) {
        if(L[i] <= R[j]) {
            arr[left+k] = L[i];
            i++;
        }
        else {
            arr[left+k] = R[j];
            j++;
        }
        k++;
    }

    while(i < l) {
        arr[left+k] = L[i];
        i++;
        k++;
    }
    while(j < r) {
        arr[left+k] = R[j];
        j++;
        k++;
    }

    free(R);
    free(L);
}

// mergesort clássico
void mergeSort(ull arr[], ull left, ull right) {
    if(left < right) {
        ull mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

// merge para as seções ordenadas pelas threads no vetor
void merge_arrays_parallel_MergeSort(ull arr[], ull n, ull aggregation, ull block_size, ull size) {
    for(ull i=0; i<n; i+=2) {
        ull left = i * (block_size * aggregation);
        ull right = ((i+2) * block_size * aggregation) -1;
        ull mid = left + (block_size * aggregation) -1;

        if(right >= size) right = size;

        merge(arr, left, mid, right);
    }

    if(n / 2 >= 1)
        merge_arrays_parallel_MergeSort(arr, n/2, aggregation*2, block_size, size);
}

// tarefa a ser executada pelas threads
void* parallel_mergeSort(void* args) {
    parameters* params = (parameters*)args;

    ull* arr = params->arr;
    ull thread_id = params->thread_id;
    ull block_size = params->size / NTHREADS;
    ull left = thread_id * (block_size);
    ull right = (thread_id + 1) * (block_size) - 1;

    if(thread_id == NTHREADS-1)
        right += (params->size % NTHREADS);

    ull mid = left + (right - left) / 2;
    if(left < right) {
        mergeSort(arr, left, right);
        mergeSort(arr, left+1, right);
        merge(arr, left, mid, right);
    }

    return NULL;
}

/* FUNÇÕES AUXILIARES */

/**
 * printa o array passado como parâmetro (recomendado para arrays pequenos)
 *
 * @param arr array de unsigned long long int
 * @param n tamanho de arr
 * @return void, apenas printa
 */
void print(ull arr[], ull n) {
    for(ull i=0; i<n; i++)
        printf("%llu ", arr[i]);
    printf("\n");
}

/**
 * verifica se o array está ordenado
 * @param arr array de unsigned long long int
 * @param n tamanho de arr
 * @return true se arr está ordenado, false caso contrário
 */
bool is_sorted(const ull arr[], ull n) {
    for(ull i=0; i<n-1; i++)
        if(arr[i] > arr[i+1])
            return false;
    return true;
}

/**
 * gera um número aleatório entre 0 e a constante MAX (inclusa)
 *
 * @return unsigned long long int aleatório
 */
ull generate_random_number() {
    return (ull)(rand() % MAX + 1);
}

/**
 * FUNÇÃO AUXILIAR USADA APENAS PARA TESTE
 * escreve em um arquivo o tamanho n do vetor e o tempo levado para ordená-lo
 * informações sobre o mergesort sequencial
 * as linhas do arquivo são do tipo (TAMANHO_DO_VETOR, TEMPO_PARA_ORDENÁ-LO)
 */
void generate_data_about_algorithm_sequential() {
    FILE *file;
    ull max = 430000000;
    ull start = (ull)pow(10,6);
    ull step = 1000000;

    for(ull i=start; i<=max; i+=step) {
        file = fopen("test_sequential.txt", "a");
        printf("i = %llu\n", i);
        ull* arr = (ull*)malloc(sizeof(ull) * i);

        double s, e, t;

        for(ull j=0; j<i; j++)
            arr[j] = generate_random_number();

        GET_TIME(s);
        mergeSort(arr, (ull)0, i-1);
        GET_TIME(e);

        t = e - s;

        fprintf(file, "(%llu, %lf)\n", i, t);
        free(arr);
        fclose(file);

        if(i == 100000000) step = 10000000;
    }

    return 0;
}

/**
 * FUNÇÃO AUXILIAR USADA APENAS PARA TESTE
 * escreve em um arquivo o tamanho n do vetor e o tempo levado para ordená-lo
 * informações sobre o mergesort concorrente
 * as linhas do arquivo são do tipo (TAMANHO_DO_VETOR, TEMPO_PARA_ORDENÁ-LO)
 */
void generate_data_about_algorithm_concurrent() {
    FILE *file;
    ull max = 430000000;
    ull start = (ull)pow(10,6);
    ull step = 1000000;

    char* path = (char*)malloc(23 * sizeof(char));
    sprintf(path, "test_with_%llu_threads.txt", NTHREADS);

    for(ull i=start; i<=max; i+=step) {
        file = fopen(path, "a");
        printf("i = %llu\n", i);
        ull* arr = (ull*)malloc(sizeof(ull) * i);

        double s, e, t;

        for(ull j=0; j<i; j++)
            arr[j] = generate_random_number();

        GET_TIME(s);
        for(ull n=0; n<NTHREADS; n++) {
            args = (parameters*) malloc(sizeof(parameters));
            args->thread_id = n;
            args->arr = arr;
            args->size = i;
            pthread_create(&threads[i], NULL, parallel_mergeSort, (void*)args));
        }

        for(ull n=0; n<NTHREADS; n++)
            pthread_join(threads[n], NULL);

        // finalizando o mergeSort concorrente
        merge_arrays_parallel_MergeSort(arr, NTHREADS, 1, i % NTHREADS, i);
        GET_TIME(e);

        t = e - s;

        fprintf(file, "(%llu, %lf)\n", i, t);
        free(arr);
        fclose(file);

        if(i == 100000000) step = 10000000;
    }

    return 0;
}

/*
 * função principal
 * gera dois vetores, ordenando um com o mergesort sequencial e o outro com o mergesort concorrente
 * verifica se estão ordenados
 * printa que estão (sempre) e diz o tempo levado em cada ordenação
 */
int main() {
    srand(time(NULL));
    ull* arr_s = (ull*) malloc(sizeof(ull) * SIZE);
    ull* arr_p = (ull*) malloc(sizeof(ull) * SIZE);

    pthread_t threads[NTHREADS];
    parameters* args;

    double start_s, end_s, time_s;
    double start_p, end_p, time_p;

    for(ull i=0; i<SIZE; i++) {
        arr_p[i] = generate_random_number();
        arr_p[i] = arr_s[i];
    }

    GET_TIME(start_p);
    for(ull i=0; i<NTHREADS; i++) {
        args = (parameters*) malloc(sizeof(parameters));
        args->thread_id = i;
        args->arr = arr_p;
        args->size = SIZE;
        if(pthread_create(&threads[i], NULL, parallel_mergeSort, (void*)args)) {
            printf("ERROR");
            exit(-1);
        }
    }

    for(ull i=0; i<NTHREADS; i++)
        pthread_join(threads[i], NULL);

    // finalizando o mergeSort concorrente
    merge_arrays_parallel_MergeSort(arr_p, NTHREADS, 1, BLOCK_SIZE, SIZE);
    GET_TIME(end_p);

    // realizando o mergeSort sequencial
    GET_TIME(start_s);
    mergeSort(arr_s, (ull)0, SIZE-1);
    GET_TIME(end_s);

    if(is_sorted(arr_p, SIZE-1) && is_sorted(arr_s, SIZE-1))
        printf("Success.\n");
    else
        printf("Error.\n");

    time_p = end_p - start_p;
    time_s = end_s - start_s;

    free(arr_p);
    free(arr_s);

    printf("Parallel (with %llu threads): %lf\n", NTHREADS, time_p);
    printf("Sequential: %lf\n", time_s);

    return 0;
}