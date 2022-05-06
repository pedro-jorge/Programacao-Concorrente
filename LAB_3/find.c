#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define ll long long int
#define MAX_FLOAT 9999

typedef struct {
    float *v;
    ll min;
    ll max;
    float* result;
} parameters;

float random_real_number(float max) {
    return (float)rand()/(float)(RAND_MAX/max);
}

void initialize_vector(float v[], ll n) {
    for(ll i=0; i<n; i++)
        v[i] = random_real_number(MAX_FLOAT);
}

float* sequential_find(float v[], ll n) {
    float menor = v[0], maior = v[0];
    float *result = malloc(sizeof(float) * 2);

    for(ll i=0; i<n; i++) {
        if(v[i] < menor) menor = v[i];
        if(v[i] > maior) maior = v[i];
    }

    result[0] = menor;
    result[1] = maior;

    return result;
}

void* concurrent_find(void* args) {
    parameters* p = (parameters*) args;
    
    for(ll i=p->min; i<p->max; i++) {
        if(p->v[i] < p->result[0]) p->result[0] = p->v[i];
        if(p->v[i] > p->result[1]) p->result[1] = p->v[i];
    }

    pthread_exit(NULL);
}

void print_vector(float v[], ll n) {
    for(ll i=0; i<n; i++)
        printf("%f\n", v[i]);
    // printf("\n");
}

int main(int argc, char* argv[]) {
    ll n = atoll(argv[1]);
    int nthreads = atoi(argv[2]);
    float v[n];
    initialize_vector(v, n);
    float result[2] = {v[0], v[0]};

    // print_vector(v, n);

    pthread_t threads_id[nthreads];
    parameters* args;
    int first = 0;
    int last = n / nthreads;

    for(int i=0; i<nthreads; i++) {
        if(i == nthreads-1) last = n;

        args = malloc(sizeof(parameters));
        args->v = v;
        args->result = result;
        args->max = last;
        args->min = first;

        pthread_create(&threads_id[i], NULL, concurrent_find, (void*)args);
        first = last;
        last = (last+(n/nthreads) <= n ? last+(n/nthreads) : n);
    }

    for(int i=0; i<nthreads; i++)
        pthread_join(threads_id[i], NULL);

    printf("menor: %f\n", result[0]);
    printf("maior: %f\n", result[1]);

    float* result_s = sequential_find(v, n);
    printf("s menor: %f\n", result_s[0]);
    printf("s maior %f\n", result_s[1]);

    return 0;
}