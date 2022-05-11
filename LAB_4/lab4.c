#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

#define ll long long int
#define MAX 99999

typedef struct {
    int id;
    ll dim;
    int* nthreads;
    int* init_array;
    float* out_array;
} parameters;

float random_integer_number(int max) {
    return rand()/(RAND_MAX/max);
}

int isPrime(int n) {
    if(n == 0 || n == 1 || n%2 == 0) return 0;

    for(int i=3; i<=(int)sqrt((double)n); i++)
        if(n%i == 0) return 0;
    
    return 1;
}

void initialize_array(int init_array[], ll dim) {
    for(ll i=0; i<dim; i++) {
        init_array[i] = random_integer_number(MAX);
    }
}

void processPrimes(int init_array[], float out_array[], int dim) {
    for(int i=0; i<dim; i++) {
        if(isPrime(init_array[i]))
            out_array[i] = sqrt(init_array[i]);
        else
            out_array[i] = init_array[i];
    }
}

void* concurrentProcessPrimes(void *args) {
    parameters *p = (parameters*)args;

    ll size = p->dim/(*p->nthreads);
    ll start = p->id * size;
    ll end;
    if(p->id == *(p->nthreads)-1) end = p->dim;
    else end = start + size;

    for(ll i=start; i<end; i++) {
        if(isPrime(p->init_array[i]))
            p->out_array[i] = sqrt(p->init_array[i]);
        else 
            p->out_array[i] = p->init_array[i]; 
    }
}

int main(int argc, char* argv[]) {
    ll dim = atoll(argv[1]);
    int nthreads = atoi(argv[2]);

    int* init_array = malloc(sizeof(int) * dim);
    float* out_array = malloc(sizeof(int) * dim);
    float* s_out_array = malloc(sizeof(int) * dim);

    double s_start, s_end, s_delta;
    double c_start, c_end, c_delta;

    initialize_array(init_array, dim);

    GET_TIME(c_start);
    pthread_t threads_id[nthreads];
    parameters* args;

    for(int i=0; i<nthreads; i++) {
        args = malloc(sizeof(parameters));
        args->id = i;
        args->dim = dim;
        args->nthreads = &nthreads;
        args->init_array = init_array;
        args->out_array = out_array;

        pthread_create(&threads_id[i], NULL, concurrentProcessPrimes, (void*)args);
    }

    for(int i=0; i<nthreads; i++)
        pthread_join(threads_id[i], NULL);

    GET_TIME(c_end);
    c_delta = c_end - c_start;

    GET_TIME(s_start);
    processPrimes(init_array, s_out_array, dim);
    GET_TIME(s_end);
    s_delta = s_end - s_start;

    // caso os resultados tenham sido diferentes
    for(ll i=0; i<dim; i++) {
        if(s_out_array[i] != out_array[i])
            printf("0");
    }

    printf("Dim: %lld, Threads: %d\n", dim, nthreads);
    printf("Tempo sequencial: %lf\n", s_delta);
    printf("Tempo concorrente: %lf\n", c_delta);

    if(s_delta > c_delta) {
        double ganho = s_delta / c_delta;
        printf("Ganho obtido: %lf", ganho);
    }
    else printf("Sem ganhos.");

    return 0;
}