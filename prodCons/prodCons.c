#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX 100000

sem_t empty;
sem_t full;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t maxCons = PTHREAD_COND_INITIALIZER;

int consumed;
int exit_threads;

struct argsStruct {
    int *argsv;
    int index;
    int maxIndex;
};

struct cancelStruct {
    pthread_t *consV;
    pthread_t *prodV;
    int Nc;
    int Np;
};


int isPrime(int num) {
    if (num <= 1) return 0;
 
    for (int i = 2; i < num; i++) {
        if (num % i == 0) return 0;
    }

    return 1;
}

void *producer(void *args) {
    while(1) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        struct argsStruct *buffer = args;
        int num = (int)(1 + (rand() % 10000000 - 1));
        buffer->argsv[buffer->index] = num;
        buffer->index++;
        if (buffer->index == buffer->maxIndex) buffer->index = 0;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *args) {
    while(1) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        struct argsStruct *buffer = args;
        int num = buffer->argsv[buffer->index];
        isPrime(num);
        // printf("%s\t", primo ? "Primo" : "Não primo");
        buffer->index++;
        if (buffer->index == buffer->maxIndex) buffer->index = 0;
        consumed++;
        if (consumed > MAX) {
            exit_threads = 1;
            pthread_cond_signal(&maxCons);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

void *cancelThreads(void *s) {
    pthread_mutex_lock(&mutex);
    struct cancelStruct *cS = s;
    while (exit_threads == 0) {
        pthread_cond_wait(&maxCons, &mutex);
    }
    for (int i = 0; i < cS->Nc; i++) {
        pthread_cancel(cS->consV[i]);
    }
    for (int i = 0; i < cS->Np; i++) {
        pthread_cancel(cS->prodV[i]);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    srand((unsigned) time(NULL));

    int N, Np, Nc;

    N = atoi(argv[1]);
    Np = atoi(argv[2]);
    Nc = atoi(argv[3]);

    double sumExc;
    int index;

    for (int iter = 0; iter < 10; iter++) {
        // pthread_t threadsProd[Np];
        // pthread_t threadsCons[Nc];
        pthread_t *threadsProd = malloc (Np * sizeof(pthread_t));
        pthread_t *threadsCons = malloc (Nc * sizeof(pthread_t));
        pthread_t cancel;

        sem_init(&empty, 0, N);
        sem_init(&full, 0, 0);

        int *v = malloc(N * sizeof(int));
        sumExc = 0.0;
        index = 0;
        consumed = 0;
        exit_threads = 0;

        struct argsStruct bufferStruct;
        bufferStruct.argsv = v;
        bufferStruct.index = index;
        bufferStruct.maxIndex = N;

        clock_t t = clock();
        for (int i = 0; i < Np; i++) {
            pthread_create(&threadsProd[i], NULL, &producer, (void *)&bufferStruct);
        }
        for (int i = 0; i < Nc; i++) {
            pthread_create(&threadsCons[i], NULL, &consumer, (void *)&bufferStruct);
        }

        struct cancelStruct cS;
        cS.consV = threadsCons;
        cS.prodV = threadsProd;
        cS.Nc = Nc;
        cS.Np = Np;
        pthread_create(&cancel, NULL, &cancelThreads, (void *)&cS);

        for (int i = 0; i < Np; i++) {
            pthread_join(threadsProd[i], NULL);
        }
        for (int i = 0; i < Nc; i++) {
            pthread_join(threadsCons[i], NULL);
        }
        pthread_join(cancel, NULL);

        double t_total = (double)(clock() - t) / CLOCKS_PER_SEC;
        sumExc += t_total;
        free(v);
        free(threadsProd);
        free(threadsCons);
        sem_destroy(&empty);
        sem_destroy(&full);
    }
    sumExc = sumExc/10.0;
    printf("\nNp: %d, Nc: %d, N: %d, t: %f\n", Np, Nc, N, sumExc);
    return 0;
}