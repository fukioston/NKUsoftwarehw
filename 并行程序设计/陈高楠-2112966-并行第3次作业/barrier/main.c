#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 8

typedef struct {
    int threadId;
} threadParm_t;
int count=0; //表示正在运行的线程数;

pthread_mutex_t barrier_mutex;

//忙等待和互斥量
void *threadFunc(void *parm) {
    threadParm_t *p = (threadParm_t *) parm;
    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
    pthread_mutex_lock(&barrier_mutex);
    count++;
    pthread_mutex_unlock(&barrier_mutex);
    while (count < NUM_THREADS);
    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&barrier_mutex, NULL);
    pthread_t thread[NUM_THREADS];
    threadParm_t threadParm[NUM_THREADS];
    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        threadParm[i].threadId = i;
        pthread_create(&thread[i], NULL, threadFunc, (void
        *) &threadParm[i]);
    }
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(&barrier_mutex);
    return 0;
}

//原始路障
//typedef struct {
//    int threadId;
//}threadParm_t;
//pthread_barrier_t barrier;
//void* threadFunc(void* parm)
//{
//    threadParm_t* p = (threadParm_t*)parm;
//    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
//    pthread_barrier_wait(&barrier);
//    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
//    pthread_exit(NULL);
//}
//int main(int argc, char* argv[])
//{
//    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
//    pthread_t thread[NUM_THREADS];
//    threadParm_t threadParm[NUM_THREADS];
//    int i;
//    for (i = 0; i < NUM_THREADS; i++)
//    {
//        threadParm[i].threadId = i;
//        pthread_create(&thread[i], NULL, threadFunc, (void
//            *)&threadParm[i]);
//    }
//    for (i = 0; i < NUM_THREADS; i++)
//    {
//        pthread_join(thread[i], NULL);
//    }
//    pthread_barrier_destroy(&barrier);
//    system("PAUSE");
//    return 0;
//}


//信号量实现路障
//int count = 0;
//typedef struct {
//    int threadId;
//} threadParm_t;
//sem_t sem_count;
//sem_t sem_barrier;
//void* threadFunc(void* parm)
//{
//    threadParm_t* p = (threadParm_t*)parm;
//    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
//    sem_wait(&sem_count);
//    if (counter == NUM_THREADS - 1)
//    {
//        counter = 0;
//        sem_post(&sem_count);
//        for (int i = 0; i < NUM_THREADS - 1; i++) sem_post(&sem_barrier);
//    }
//    else
//    {
//        count++;
//        sem_post(&sem_count);
//        sem_wait(&sem_barrier);
//    }
//
//    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
//    pthread_exit(NULL);
//}
//int main(int argc, char* argv[])
//{
//    sem_init(&sem_count, 0, 1);
//    sem_init(&sem_barrier, 0, 0);
//    pthread_t thread[NUM_THREADS];
//    threadParm_t threadParm[NUM_THREADS]; //´«µÝ²ÎÊý
//    int i;
//    for (i = 0; i < NUM_THREADS; i++)
//    {
//        threadParm[i].threadId = i;
//        pthread_create(&thread[i], NULL, threadFunc, (void
//            *)&threadParm[i]);
//    }
//    for (i = 0; i < NUM_THREADS; i++)
//    {
//        pthread_join(thread[i], NULL);
//    }
//    sem_destroy(&sem_count);
//    sem_destroy(&sem_barrier);
//    return 0;
//}