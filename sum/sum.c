#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

pthread_spinlock_t lock;

void acquire() {
    pthread_spin_lock(&lock);
}

void release() {
    pthread_spin_unlock(&lock);
}

int sum = 0;

struct argsStruct {
    char *argsv;
    int initialIndex;
    int finalIndex;
};

void *sumV(void *arguments){
    struct argsStruct *args = arguments;
    int temp = 0;
    for (int j = args->initialIndex; j <= args->finalIndex; j++) {
        temp += (int)args->argsv[j];
    }
    acquire();
    sum += temp;
    release();
    pthread_exit(NULL);
}

int resetSum() {
    acquire();
    int returnSum = sum;
    sum = 0;
    release();
    return returnSum;
}

int main(void) {
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    int N;
    printf("Tamanho do vetor: ");
    scanf("%d", &N);
    int K;
    printf("Numero de threads: ");
    scanf("%d", &K);

	pthread_t threadsSum[K];
    char *v = malloc(N * 1);
    int subArrLen = N/K;
    printf("Tamanho subVs: %d\n", subArrLen);
    struct argsStruct arrArgs[K];

    srand((unsigned) time(NULL));
    for (int i = 0; i < N; i++) {
        v[i] = (char)(rand() % 101) * pow((-1),(rand() % 2));
        // printf("%d ", (int)v[i]);
    }
    printf("\n");

    for (int i = 0; i < K; i++) {
        int initialIndex = subArrLen * i;
        int finalIndex = initialIndex + subArrLen - 1;
        finalIndex = i == (K-1) ? (N-1) : finalIndex;
        arrArgs[i].argsv = v;
        arrArgs[i].initialIndex = initialIndex;
        arrArgs[i].finalIndex = finalIndex;
    }

    clock_t t = clock();
	for (int i = 0; i < K; i++) {
        pthread_create(&threadsSum[i], NULL, &sumV, (void *)&arrArgs[i]);
    }
    for (int i = 0 ; i < K ; i++) {
        pthread_join(threadsSum[i], NULL);
    }
    double t_threads = (double)(clock() - t) / CLOCKS_PER_SEC;

    printf("Tempo multithreads: %f\n", t_threads);
    int sumThreads = resetSum();
    printf("Soma multithreads: %d\n", sumThreads);

    pthread_t totalSum;
    struct argsStruct args;
    args.argsv = v;
    args.initialIndex = 0;
    args.finalIndex = (N-1);
    t = clock();
    pthread_create(&totalSum, NULL, &sumV, (void *)&args);
    pthread_join(totalSum, NULL);
    double t_total = (double)(clock() - t) / CLOCKS_PER_SEC;

    printf("Tempo uma thread: %f\n", t_total);
    int sumTotal = resetSum();
    printf("Soma uma thread: %d\n", sumTotal);

	return 0;
}