#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 5

int count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void* thread1(void* args) {
    pthread_mutex_lock(&mutex);
    while(count != 4) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Volte sempre!\n");
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}

void* thread2(void* args) {
    pthread_mutex_lock(&mutex);

    while(count < 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    count++;
    printf("Fique a vontade.\n");
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}

void* thread3(void* args) {
    pthread_mutex_lock(&mutex);

    while(count < 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    count++;
    printf("Sente-se por favor.\n");
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}

void* thread4(void* args) {
    pthread_mutex_lock(&mutex);

    while(count < 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    count++;
    printf("Aceita um copo d'agua?\n");
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}

void* thread5(void* args) {
    pthread_mutex_lock(&mutex);

    while(count != 0) {
        pthread_cond_wait(&cond, &mutex);
    }

    count++;
    printf("Seja bem-vindo!\n");
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);
}

int main() {
    void* args = (void*)malloc(sizeof(void));

    pthread_t threads[NTHREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&threads[0], NULL, thread1, args);
    pthread_create(&threads[1], NULL, thread2, args);
    pthread_create(&threads[2], NULL, thread3, args);
    pthread_create(&threads[3], NULL, thread4, args);
    pthread_create(&threads[4], NULL, thread5, args);

    for(int i=0; i<NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}
