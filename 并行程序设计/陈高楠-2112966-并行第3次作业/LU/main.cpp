#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include <thread>
#include<pthread.h>
#include <immintrin.h>
#include<semaphore.h>
using namespace std;
const int maxN = 10000;
const int N = 4;
const int THREAD_NUM = 8;
float a[maxN][maxN];
float b[maxN][maxN];
float c[maxN][maxN];
float d[maxN][maxN];
float answer[maxN];
float x[maxN];
sem_t sem_main;  //信号量
sem_t sem_workstart[THREAD_NUM];
sem_t sem_workend[THREAD_NUM];

sem_t sem_leader;
sem_t sem_Division[THREAD_NUM];
sem_t sem_Elimination[THREAD_NUM];

pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;
struct threadParam_t {    //参数数据结构
    int k;
    int t_id;
    int chunk_size;
    int n;
};
double LU0(int n, float a[][maxN]) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = 0; i <= n - 1; i++) //对每一行
    {
        for (int j = i + 1;j <= n;j++)//对这一行的每一个数
        {
            a[i][j] = a[i][j] / a[i][i];
        }
        a[i][i] = 1.0;
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {
            float tem = a[j][i] / a[i][i];
            for (int k = i + 1; k <= n; k++) //这个是列，列是n+1个（有b）
            {
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "普通方法耗时: " << time << "s" << endl;
    return time;


}
void* LU_pthread(void* param) {
     threadParam_t* p = (threadParam_t*)param;
    int k = p->k;           //第k步
    int t_id = p->t_id;     //第几个线程
    int c_size=p->chunk_size;//一个线程执行几行
    int start = k + t_id*c_size+1 ;   //获取任务
    int n=p->n;
    for(int j=start;(j<start+c_size)&&(j<n);j++)
    {
         for(int m=k+1;m<=n;m++)
    {
        a[j][m]-=a[k][m]*a[j][k];

    }
    a[j][k]=0.0;
    }

    pthread_exit(NULL);
    return NULL;
}
double LU(int n, float a[][maxN]) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = 0; i < n; i++) //对每一行
    {
        for (int j = i + 1;j <= n;j++)//对这一行的每一个数
        {
            a[i][j] = a[i][j] / a[i][i];
        }
        a[i][i] = 1.0;

        int thread_cnt = THREAD_NUM;
        int thread_size=(n-(i+1))/THREAD_NUM+1;
        pthread_t* thread_handles = (pthread_t*)malloc(thread_cnt * sizeof(pthread_t));
        threadParam_t* param = (threadParam_t*)malloc(thread_cnt * sizeof(threadParam_t));
        for (int t_id = 0; t_id < thread_cnt; t_id++) {//分配任务
            param[t_id].k = i;//到i步
            param[t_id].t_id = t_id;//第几个线程
            param[t_id].chunk_size=thread_size;//每个片分几个
            param[t_id].n=n;
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_create(&thread_handles[t_id], NULL, LU_pthread, &param[t_id]);
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_join(thread_handles[t_id], NULL);
        }
        free(thread_handles);
        free(param);
    }


    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "普通Pthread方法耗时: " << time << "s" << endl;
    return time;


}
int next_arr=0;
pthread_mutex_t mutex_task;
void* LU_pthread2(void* param) {
     threadParam_t* p = (threadParam_t*)param;
    int k = p->k;           //第k步
    int t_id = p->t_id;     //第几个线程
    int c_size=p->chunk_size;//一个线程执行几行
    int start = k + t_id*c_size+1 ;   //获取任务
    int n=p->n;
    int task=0;
    while (1) {
		pthread_mutex_lock(&mutex_task);
		task = next_arr;
		next_arr+=1;
		pthread_mutex_unlock(&mutex_task);
		if (task >= n) break;
		int end_task=task+1;
    for(int j = task; j < end_task; j++)
    {
         for(int m=k+1;m<=n;m++)
    {
        a[j][m]-=a[k][m]*a[j][k];

    }
    a[j][k]=0.0;
    }
}
    pthread_exit(NULL);
    return NULL;
}
double LU2(int n, float a[][maxN]) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = 0; i < n; i++) //对每一行
    {
        for (int j = i + 1;j <= n;j++)//对这一行的每一个数
        {
            a[i][j] = a[i][j] / a[i][i];
        }
        a[i][i] = 1.0;
        next_arr=i+1;
        int thread_cnt = THREAD_NUM;
        pthread_t* thread_handles = (pthread_t*)malloc(thread_cnt * sizeof(pthread_t));
        threadParam_t* param = (threadParam_t*)malloc(thread_cnt * sizeof(threadParam_t));
        for (int t_id = 0; t_id < thread_cnt; t_id++) {//分配任务
            param[t_id].k = i;//到i步
            param[t_id].t_id = t_id;//第几个线程
            param[t_id].n=n;
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_create(&thread_handles[t_id], NULL, LU_pthread2, &param[t_id]);
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_join(thread_handles[t_id], NULL);
        }
        free(thread_handles);
        free(param);
    }


    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "普通Pthread方法耗时: " << time << "s" << endl;
    return time;


}

void* LU_SSE_pthread(void* param) {
     threadParam_t* p = (threadParam_t*)param;
    int k = p->k;           //第k步
    int t_id = p->t_id;     //第几个线程
    int c_size=p->chunk_size;//一个线程执行几行
    int start = k + t_id*c_size+1 ;   //获取任务
    int n=p->n;
    __m128 t1, t2, sub, tem2;
    for(int j=start;(j<start+c_size)&&(j<n);j++)
    {   float tem = a[j][k];
            tem2 = _mm_set1_ps(tem);
         for(int m=k+1;m<=n;m+=4)
    {
        if (m + 3 > n)break;
                t1 = _mm_loadu_ps(a[k] + m);
                t2 = _mm_loadu_ps(a[j] + m);
                sub = _mm_sub_ps(t2, _mm_mul_ps(t1, tem2));
                _mm_storeu_ps(a[j] + m, sub);


    }
    for (int m = n - (n - k) % 4 + 1; m <= n; m += 1) {
                a[j][m]-=a[k][m]*a[j][k];
            }

    a[j][k]=0.0;
    }

    pthread_exit(NULL);
    return NULL;
}
double LU_SSE(int n, float a[][maxN])
{
     LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = 0; i < n; i++) //对每一行
    {
         __m128 vt = _mm_set1_ps(a[i][i]);
        for (int j = i + 1; j  <= n; j += 4) {
                if(j+3>n)break;
            __m128 va = _mm_loadu_ps(&a[i][j]);
            va = _mm_div_ps(va, vt);
            _mm_storeu_ps(&a[i][j], va);
        }
        for (int j=n-(n-i)%4+1; j <=n; j++) {
            a[i][j] = a[i][j] / a[i][i];
        }
       a[i][i] = 1.0;

        int thread_cnt = THREAD_NUM;
        int thread_size=(n-(i+1))/THREAD_NUM+1;
        pthread_t* thread_handles = (pthread_t*)malloc(thread_cnt * sizeof(pthread_t));
        threadParam_t* param = (threadParam_t*)malloc(thread_cnt * sizeof(threadParam_t));
        for (int t_id = 0; t_id < thread_cnt; t_id++) {//分配任务
            param[t_id].k = i;//到i步
            param[t_id].t_id = t_id;//第几个线程
            param[t_id].chunk_size=thread_size;//每个片分几个
            param[t_id].n=n;
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_create(&thread_handles[t_id], NULL, LU_SSE_pthread, &param[t_id]);
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_join(thread_handles[t_id], NULL);
        }
        free(thread_handles);
        free(param);
    }


    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "SSE和Pthread方法耗时: " << time << "s" << endl;
    return time;

}


void* LU_AVX_pthread(void* param) {
     threadParam_t* p = (threadParam_t*)param;
    int k = p->k;           //第k步
    int t_id = p->t_id;     //第几个线程
    int c_size=p->chunk_size;//一个线程执行几行
    int start = k + t_id*c_size+1 ;   //获取任务
    int n=p->n;
    __m256 t1, t2, sub, tem2;
    for(int j=start;(j<start+c_size)&&(j<n);j++)
    {   float tem = a[j][k];
            tem2 = _mm256_set1_ps(tem);
         for(int m=k+1;m<=n;m+=8)
    {
        if (m + 7 > n)break;
                t1 = _mm256_loadu_ps(a[k] + m);
                t2 = _mm256_loadu_ps(a[j] + m);
                sub = _mm256_sub_ps(t2, _mm256_mul_ps(t1, tem2));
                _mm256_storeu_ps(a[j] + m, sub);


    }
    for (int m = n - (n - k) % 8 + 1; m <= n; m += 1) {
                a[j][m]-=a[k][m]*a[j][k];
            }

    a[j][k]=0.0;
    }

    pthread_exit(NULL);
    return NULL;
}
double LU_AVX(int n, float a[][maxN])
{
     LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = 0; i < n; i++) //对每一行
    {

        __m256 vt = _mm256_set1_ps(a[i][i]);
        for (int j = i + 1; j  <= n; j += 8) {
                if(j+7>n)break;
            __m256 va = _mm256_loadu_ps(&a[i][j]);
            va = _mm256_div_ps(va, vt);
            _mm256_storeu_ps(&a[i][j], va);
        }
        for (int j=n-(n-i)%8+1; j <=n; j++) {
            a[i][j] = a[i][j] / a[i][i];
        }
       a[i][i] = 1.0;

        int thread_cnt = THREAD_NUM;
        int thread_size=(n-(i+1))/THREAD_NUM+1;
        pthread_t* thread_handles = (pthread_t*)malloc(thread_cnt * sizeof(pthread_t));
        threadParam_t* param = (threadParam_t*)malloc(thread_cnt * sizeof(threadParam_t));
        for (int t_id = 0; t_id < thread_cnt; t_id++) {//分配任务
            param[t_id].k = i;//到i步
            param[t_id].t_id = t_id;//第几个线程
            param[t_id].chunk_size=thread_size;//每个片分几个
            param[t_id].n=n;
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_create(&thread_handles[t_id], NULL, LU_AVX_pthread, &param[t_id]);
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_join(thread_handles[t_id], NULL);
        }
        free(thread_handles);
        free(param);
    }


    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "AVX和Pthread方法耗时: " << time << "s" << endl;
    return time;

}



void* LU_AVX_pthread_sem(void* param) {
   threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;
    int n=p->n;
    for (int k = 0; k < n; k++) {
        sem_wait(&sem_workstart[t_id]);//阻塞，等待主线程除法完成

        for (int i = k + 1 + t_id; i < n; i += THREAD_NUM) {
            __m256 vaik = _mm256_set1_ps(a[i][k]);
            int j;
            for (int j = k + 1; j <= n; j += 8) {
                    if(j+7>n)break;
                __m256 vakj = _mm256_loadu_ps(&a[k][j]);
                __m256 vaij = _mm256_loadu_ps(&a[i][j]);
                __m256 vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_storeu_ps(&a[i][j], vaij);
            }
            for (int j =n-(n-k)%8+1;j <=n; j++) {
                a[i][j] =a[i][j] - a[i][k] * a[k][j];
            }
            a[i][k] = 0.0;
        }
        sem_post(&sem_main);        //唤醒主线程
        sem_wait(&sem_workend[t_id]);  //阻塞，等待主线程唤醒进入下一轮

    }
    pthread_exit(NULL);
    return NULL;
}
double LU_AVX_sem(int n, float a[][maxN])
{
     LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
   sem_init(&sem_main, 0, 0); //初始化信号量
    for (int i = 0; i < THREAD_NUM; i++) {
        sem_init(&sem_workend[i], 0, 0);
        sem_init(&sem_workstart[i], 0, 0);
    }
    pthread_t* handle = (pthread_t*)malloc(THREAD_NUM * sizeof(pthread_t));
    threadParam_t* param = (threadParam_t*)malloc(THREAD_NUM * sizeof(threadParam_t));
    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        param[t_id].t_id = t_id;
        param[t_id].k = 0;
        param[t_id].n=n;
        pthread_create(&handle[t_id], NULL, LU_AVX_pthread_sem, &param[t_id]);

    }

    for (int k = 0; k < n; k++) {

        __m256 vt = _mm256_set1_ps(a[k][k]);

        for (int j = k + 1; j  <= n; j += 8) {
                if(j+7>n)break;
            __m256 va = _mm256_loadu_ps(&a[k][j]);
            va = _mm256_div_ps(va, vt);
            _mm256_storeu_ps(&a[k][j], va);
        }
        for (int j=n-(n-k)%8+1 ; j <= n; j++) {
            a[k][j] = a[k][j] / a[k][k];
        }
       a[k][k] = 1.0;

        for (int t_id = 0; t_id < THREAD_NUM; t_id++) {  //唤起子线程
            sem_post(&sem_workstart[t_id]);
        }

        for (int t_id = 0; t_id < THREAD_NUM; t_id++) {  //主线程睡眠
            sem_wait(&sem_main);
        }

        for (int t_id = 0; t_id < THREAD_NUM; t_id++) {  //再次唤起工作线程，进入下一轮消去
            sem_post(&sem_workend[t_id]);
        }

    }
    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        pthread_join(handle[t_id], NULL);
    }
    sem_destroy(&sem_main);    //销毁线程
    for (int t_id = 0; t_id < THREAD_NUM; t_id++)
        sem_destroy(&sem_workstart[t_id]);
    for (int t_id = 0; t_id < THREAD_NUM; t_id++)
        sem_destroy(&sem_workend[t_id]);

    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "AVX结合Pthread结合信号量方法耗时: " << time << "s" << endl;
    return time;

}

void* LU_AVX_pthread_barrier(void* param) {
   threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;
    int n=p->n;
   for (int k = 0; k <n; k++) { //0号线程做除法
        if (t_id == 0) {
            __m256 vt = _mm256_set1_ps(a[k][k]);
            for (int j = k + 1; j <= n; j += 8) {
                     if(j+7>n)break;
                __m256 va = _mm256_loadu_ps(&a[k][j]);
                va = _mm256_div_ps(va, vt);
                _mm256_storeu_ps(&a[k][j], va);
            }
            for (int j=n-(n-k)%8+1 ; j <= n;  j++) {
                a[k][j] =a[k][j] / a[k][k];
            }
            a[k][k] = 1.0;
        }

        pthread_barrier_wait(&barrier_Division);//第一个同步点

        for (int i = k + 1 + t_id; i < n; i += THREAD_NUM) {
            __m256 vaik = _mm256_set1_ps(a[i][k]);

            for (int j = k + 1; j <=n ; j += 8) {
                    if (j + 7 > n)break;
                __m256 vakj = _mm256_loadu_ps(&a[k][j]);
                __m256 vaij = _mm256_loadu_ps(&a[i][j]);
                __m256 vx = _mm256_mul_ps(vakj, vaik);
                vaij = _mm256_sub_ps(vaij, vx);
                _mm256_storeu_ps(&a[i][j], vaij);
            }
            for (int j  = n - (n - k) % 8 + 1; j <= n; j++) {
                a[i][j] = a[i][j] - a[i][k] * a[k][j];
            }
            a[i][k] = 0.0;
        }

        pthread_barrier_wait(&barrier_Elimination);//第二个同步点


    }
    pthread_exit(NULL);
    return NULL;
}
double LU_AVX_barrier(int n, float a[][maxN])
{
     LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
  pthread_barrier_init(&barrier_Division, NULL,THREAD_NUM);
    pthread_barrier_init(&barrier_Elimination, NULL, THREAD_NUM);
    pthread_t* handle = (pthread_t*)malloc(THREAD_NUM * sizeof(pthread_t));
    threadParam_t* param = (threadParam_t*)malloc(THREAD_NUM * sizeof(threadParam_t));
    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        param[t_id].t_id = t_id;
        param[t_id].k = 0;
        param[t_id].n=n;
        pthread_create(&handle[t_id], NULL,LU_AVX_pthread_barrier, &param[t_id]);

    }

    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        pthread_join(handle[t_id], NULL);
    }

    pthread_barrier_destroy(&barrier_Division);
    pthread_barrier_destroy(&barrier_Elimination);

    free(handle);
    free(param);
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "AVX，Pthread，屏障结合方法耗时: " << time << "s" << endl;
    return time;

}

void* LU_AVX_pthread_barrier2(void* param) {
   threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;
    int n=p->n;
   for (int k = 0; k < n; k++) { //0号线程做除法
        if (t_id == 0) {
            __m256 vt = _mm256_set1_ps(a[k][k]);
            for (int j = k + 1; j <= n; j += 8) {
                     if(j+7>n)break;
                __m256 va = _mm256_loadu_ps(&a[k][j]);
                va = _mm256_div_ps(va, vt);
                _mm256_storeu_ps(&a[k][j], va);
            }
            for (int j=n-(n-k)%8+1 ; j <=n;  j++) {
                a[k][j] =a[k][j] / a[k][k];
            }
            a[k][k] = 1.0;
        }
        next_arr=k+1;
        int task=0;
        __m256 t1, t2, sub, tem2;
        pthread_barrier_wait(&barrier_Division);//第一个同步点


            while(1)
            {
                pthread_mutex_lock(&mutex_task);
                task = next_arr;
                next_arr+=1;
                pthread_mutex_unlock(&mutex_task);
                if (task >= n) break;
                int end_task=task+1;
                if(end_task>n)end_task=n;
                for(int j = task; j < end_task; j++)
              {   tem2 = _mm256_set1_ps(a[j][k]);
                    for(int m=k+1;m<=n;m+=8)
    {
                if (m + 7 > n)break;
                t1 = _mm256_loadu_ps(a[k] + m);
                t2 = _mm256_loadu_ps(a[j] + m);
                sub = _mm256_sub_ps(t2, _mm256_mul_ps(t1, tem2));
                _mm256_storeu_ps(a[j] + m, sub);


    }
    for (int m = n - (n - k) % 8 + 1; m <= n; m += 1) {
                a[j][m]-=a[k][m]*a[j][k];
            }

                a[j][k]=0.0;

              }
            }

        pthread_barrier_wait(&barrier_Elimination);//第二个同步点


    }
    pthread_exit(NULL);
    return NULL;

}
double LU_AVX_barrier2(int n, float a[][maxN])
{
     LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
  pthread_barrier_init(&barrier_Division, NULL,THREAD_NUM);
    pthread_barrier_init(&barrier_Elimination, NULL, THREAD_NUM);
    pthread_t* handle = (pthread_t*)malloc(THREAD_NUM * sizeof(pthread_t));
    threadParam_t* param = (threadParam_t*)malloc(THREAD_NUM * sizeof(threadParam_t));
    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        param[t_id].t_id = t_id;
        param[t_id].k = 0;
        param[t_id].n=n;
        pthread_create(&handle[t_id], NULL,LU_AVX_pthread_barrier2, &param[t_id]);

    }

    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        pthread_join(handle[t_id], NULL);
    }

    pthread_barrier_destroy(&barrier_Division);
    pthread_barrier_destroy(&barrier_Elimination);

    free(handle);
    free(param);
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "AVX，Pthread，屏障结合动态分配任务方法耗时: " << time << "s" << endl;
    return time;

}



double generation(int n, float a[][maxN], float x[])
{
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = a[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= a[i][j] * x[j];
        }
        x[i] /= a[i][i];
    }
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "普通方法回代: " << time << "s" << endl;
    return time;

}

int main()
{
  int n;
    n = 128;
    int x = 0;


    double result[100][8];
    for (int i = 0;i < 100;i++) {
        for (int j = 0;j < 7;j++)
            result[i][j] = 0;
    }
    while (n <= 2560)
    {

        cout << "矩阵规模为" << n << "*" << n << "时" << endl;
        x = 0;


        while (x++ < 20)
        {
            cout << "第" << x << "次高斯消元" << endl;
            cout << endl;
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n + 1; j++) {
                    a[i][j] = (rand() % 100000)/ 100.00;

                }

            }
            result[n / 128][0] += LU_AVX_barrier2(n, a);



        }
        result[n / 128][0] /= 20;


        cout << result[n / 128][0] << "  " << endl;


        n += 128;
        cout << endl;

    }
    return 0;



// for (int i = 0;i < 10;i++)
//        for (int j = 0;j < 11;j++)
//        {
//            cin >> a[i][j];
//        }
//    LU_AVX_barrier2(10, a);
//    for (int i = 0;i < 10;i++)
//    {
//        for (int j = 0;j < 11;j++)
//        {
//            cout << a[i][j] << " ";
//            b[i][j] = a[i][j];
//            c[i][j] = a[i][j];
//        }
//        cout << endl;
//
//    }
//
//
//
//    generation(10, a, x);
//    for (int i = 0;i < 10;i++) {
//        cout << x[i] << " ";
//        x[i] = 0;
//    }
//    cout << endl;



}
