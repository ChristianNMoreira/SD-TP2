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

// Estrutura para os argumentos das threads
struct ArgsStruct {
    char *vector;
    int initialIndex;
    int finalIndex;
};

// Função executada pelas threads para calcular a soma do vetor
void *sumVector(void *arguments){
    struct ArgsStruct *args = arguments;
    int temp = 0;
    for (int j = args->initialIndex; j <= args->finalIndex; j++) {
        temp += (int)args->vector[j];
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
    int vectorSize;
    printf("Tamanho do vetor: ");
    scanf("%d", &vectorSize);
    int numThreads;
    printf("Numero de threads: ");
    scanf("%d", &numThreads);

    pthread_t threadsSum[numThreads];
    char *vector = malloc(vectorSize * sizeof(char));
    int subVectorLength = vectorSize / numThreads;
    printf("Tamanho do subvetor: %d\n", subVectorLength);
    struct ArgsStruct threadArgs[numThreads];

    srand((unsigned) time(NULL));
    for (int i = 0; i < vectorSize; i++) {
        vector[i] = (char)(rand() % 101) * pow((-1),(rand() % 2));
        // printf("%d ", (int)vector[i]);
    }
    printf("\n");

    // Divisão do vetor em subvetores para cada thread
    for (int i = 0; i < numThreads; i++) {
        int initialIndex = subVectorLength * i;
        int finalIndex = initialIndex + subVectorLength - 1;
        finalIndex = i == (numThreads-1) ? (vectorSize-1) : finalIndex;
        threadArgs[i].vector = vector;
        threadArgs[i].initialIndex = initialIndex;
        threadArgs[i].finalIndex = finalIndex;
    }

    clock_t t = clock();

    // Criação das threads para calcular a soma do vetor em paralelo
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threadsSum[i], NULL, &sumVector, (void *)&threadArgs[i]);
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threadsSum[i], NULL);
    }

    double t_threads = (double)(clock() - t) / CLOCKS_PER_SEC;

    printf("Tempo com threads: %f\n", t_threads);
    int sumThreads = resetSum();
    printf("Soma com threads: %d\n", sumThreads);

    pthread_t totalSum;
    struct ArgsStruct args;
    args.vector = vector;
    args.initialIndex = 0;
    args.finalIndex = (vectorSize - 1);
    t = clock();

        // Criação da thread para calcular a soma do vetor usando uma única thread
    pthread_create(&totalSum, NULL, &sumVector, (void *)&args);
    pthread_join(totalSum, NULL);
    double t_total = (double)(clock() - t) / CLOCKS_PER_SEC;

    printf("Tempo com uma thread: %f\n", t_total);
    int sumTotal = resetSum();
    printf("Soma com uma thread: %d\n", sumTotal);

    return 0;
}
