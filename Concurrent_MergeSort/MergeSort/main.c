#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"

#define ull unsigned long long int
#define MAX 99999

const ull SIZE = 10000;
const ull NTHREADS = 4;
const ull BLOCK_SIZE = SIZE / NTHREADS;
const ull OFFSET = SIZE % NTHREADS;

typedef struct {
    ull* arr;
    ull thread_id;
} parameters;

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

void mergeSort(ull arr[], ull left, ull right) {
    if(left < right) {
        ull mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

void merge_arrays_parallel_MergeSort(ull arr[], ull n, ull aggregation) {
    for(ull i=0; i<n; i+=2) {
        ull left = i * (BLOCK_SIZE * aggregation);
        ull right = ((i+2) * BLOCK_SIZE * aggregation) -1;
        ull mid = left + (BLOCK_SIZE * aggregation) -1;

        if(right >= SIZE) right = SIZE-1;

        merge(arr, left, mid, right);
    }

    if(n / 2 >= 1)
        merge_arrays_parallel_MergeSort(arr, n/2, aggregation*2);
}

void* parallel_mergeSort(void* args) {
    parameters* params = (parameters*)args;

    ull* arr = params->arr;
    ull thread_id = params->thread_id;
    ull left = thread_id * (BLOCK_SIZE);
    ull right = (thread_id + 1) * (BLOCK_SIZE) - 1;

    if(thread_id == NTHREADS-1)
        right += OFFSET;

    ull mid = left + (right - left) / 2;
    if(left < right) {
        mergeSort(arr, left, right);
        mergeSort(arr, left+1, right);
        merge(arr, left, mid, right);
    }

    return NULL;
}

void print(ull arr[], ull n) {
    for(ull i=0; i<n; i++)
        printf("%llu ", arr[i]);
    printf("\n");
}

bool is_sorted(const ull arr[], ull n) {
    for(ull i=0; i<n-1; i++)
        if(arr[i] > arr[i+1])
            return false;
    return true;
}

ull generate_random_number() {
    return (ull)(rand() % MAX + 1);
}

int main() {
    srand(time(NULL));
    double start_s, end_s, time_s;
    FILE *file = fopen("time_sequential.txt", "a");

    ull* arr_s = (ull*)malloc(sizeof(ull) * SIZE);
    ull MAX_SIZE = SIZE+100;
    ull MILLION = 1000000;
    for(ull i=SIZE; i<MAX_SIZE; i++) {
        if(i > SIZE) arr_s = (ull*)realloc(arr_s, sizeof(ull) * i);
        for(ull j=0; j<i; j++)
            arr_s[i] = generate_random_number();

        GET_TIME(start_s);
        //mergeSort(arr_s, (ull)0, i-1);
        GET_TIME(end_s);

        time_s = end_s - start_s;

        fprintf(file, "(%llu, %lf)", i, time_s);
    }

    fclose(file);
    return 0;
}
/*
int main() {
    srand(time(NULL));
    ull* arr_s = (ull*) malloc(sizeof(ull) * SIZE);
    ull* arr_p = (ull*) malloc(sizeof(ull) * SIZE);

    pthread_t threads[NTHREADS];
    parameters* args;

    double start_s, end_s, time_s;
    double start_p, end_p, time_p;

    for(ull i=0; i<SIZE; i++) {
        arr_s[i] = generate_random_number();
        arr_p[i] = arr_s[i];
    }

    GET_TIME(start_p);
    for(ull i=0; i<NTHREADS; i++) {
        args = (parameters*) malloc(sizeof(parameters));
        args->thread_id = i;
        args->arr = arr_p;
        if(pthread_create(&threads[i], NULL, parallel_mergeSort, (void*)args)) {
            printf("ERROR");
            exit(-1);
        }
    }

    for(ull i=0; i<NTHREADS; i++)
        pthread_join(threads[i], NULL);

    // finalizando o mergeSort concorrente
    merge_arrays_parallel_MergeSort(arr_p, NTHREADS, 1);
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

    printf("Parallel (with %llu threads): %lf\n", NTHREADS, time_p);
    printf("Sequential: %lf\n", time_s);
    return 0;
}
 */