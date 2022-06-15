#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define L 4
#define E 2

int leit = 0;
int escr = 0;

pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;

void InicLeit(int id) {
    pthread_mutex_lock(&mutex);
    printf("L[%d] quer ler\n", id);
    while(escr > 0) {
        printf("L[%d] bloqueou\n", id);
        pthread_cond_wait(&cond_leit, &mutex);
        printf("L[%d] desbloqueou\n", id);
    }
    leit++;
    pthread_mutex_unlock(&mutex);
}

void FimLeit(int id) {
    pthread_mutex_lock(&mutex);
    printf("L[%d] terminou de ler\n", id);
    leit--;
    pthread_cond_signal(&cond_leit);
    pthread_cond_broadcast(&cond_escr);
    pthread_mutex_unlock(&mutex);
}

void InicEscr(int id) {
    pthread_mutex_lock(&mutex);
    printf("E[%d] quer escrever\n", id);
    while(leit > 0) {
        printf("E[%d] bloqueou\n", id);
        pthread_cond_wait(&cond_escr, &mutex);
        printf("E[%d] desbloqueou\n", id);
    }
    escr++;
    pthread_mutex_unlock(&mutex);
}

void FimEscr(int id) {
    pthread_mutex_lock(&mutex);
    printf("E[%d] terminou de escrever\n", id);
    escr--;
    if(escr == 0) pthread_cond_signal(&cond_leit);
    pthread_mutex_unlock(&mutex);
}

void* leitor(void *arg) {
    int *id = (int*)arg;
    while(1) {
        InicLeit(*id);
        printf("Leitora %d esta lendo\n", *id);
        FimLeit(*id);
        sleep(1);
    }
    free(arg);
    pthread_exit(NULL);
}

void* escritor(void* arg) {
    int *id = (int*)arg;
    while(1) {
        InicEscr(*id);
        printf("Escritora %d esta escrevendo\n", *id);
        FimEscr(*id);
        sleep(1);
    }
    free(arg);
    pthread_exit(NULL);
}

int main() {
    pthread_t tid[L+E];
    int id[L+E];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_leit, NULL);
    pthread_cond_init(&cond_escr, NULL);

    for(int i=0; i<L; i++) {
        id[i] = i+1;
        if(pthread_create(&tid[i], NULL, leitor, (void*)&id[i])) exit(-1);
    }

    for(int i=0; i<E; i++) {
        id[i+L] = i+1;
        if(pthread_create(&tid[i], NULL, escritor, (void*)&id[i])) exit(-1);
    }

    pthread_exit(NULL);
    return 0;
}
