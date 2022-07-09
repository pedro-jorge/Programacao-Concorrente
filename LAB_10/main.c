#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

const unsigned int UINT_MAX = 4294967295;

const int L = 4;
const int E = 3;
const int NTHREADS = L+E;

sem_t em_e, em_l, escr, leit;
int e = 0, l = 0;

void* leitor(void* thread_id) {
    int id = *(int*)thread_id;

    while(1) {
        sem_wait(&leit);
        sem_wait(&em_l);
        l++;
        if(l == 1) sem_wait(&escr);
        sem_post(&em_l);
        sem_post(&leit);

         /* (reading)
         * doing something to take time */
         printf("Thread %d (leitora) lendo\n", id);
        unsigned int placebo = 0;
        for(unsigned int i=0; i<UINT_MAX; i++) {
            placebo++;
        }

        sem_wait(&em_l);
        l--;
        if(l == 0) sem_post(&escr);
        sem_post(&em_l);
    }
}

void* escritor(void* thread_id) {
    int id = *(int*)thread_id;

    while(1) {
        sem_wait(&em_e);
        e++;
        if(e == 1) sem_wait(&leit);
        sem_post(&em_e);
        sem_wait(&escr);

        /* writing
         * doing something to take time */
        printf("Thread %d (escritora) escrevendo\n", id);
        unsigned int placebo = 0;
        for(unsigned int i=0; i<UINT_MAX/100; i++) {
            placebo++;
        }

        sem_post(&escr);
        sem_wait(&em_e);
        e--;
        if(e == 0) sem_post(&leit);
        sem_post(&em_e);
        sleep(1);
    }
}

int main() {
    pthread_t threads[NTHREADS];
    int ids[NTHREADS];
    int i;

    sem_init(&leit, 0, 1);
    sem_init(&em_l, 0, 1);

    sem_init(&escr, 0, 1);
    sem_init(&em_e, 0, 1);

    printf("Initializando...\n");
    /* initializing writers */
    for(i=0; i<E; i++) {
        printf("Thread %d (escritora) criada\n", i);
        ids[i] = i;
        pthread_create(&threads[i], NULL, escritor, (void*)&ids[i]);
    }

    /* initializing readers */
    for(i; i<NTHREADS; i++) {
        printf("Thread %d (leitora) criada\n", i);
        ids[i] = i;
        pthread_create(&threads[i], NULL, leitor, (void*)&ids[i]);
    }

    for(i=0; i<NTHREADS; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
