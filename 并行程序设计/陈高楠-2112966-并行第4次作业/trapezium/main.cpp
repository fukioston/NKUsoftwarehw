#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include <thread>
#include<pthread.h>
#include <immintrin.h>
#include<semaphore.h>
#include<omp.h>
using namespace std;
pthread_mutex_t barrier_mutex;
struct threadParam_t {    //参数数据结构
    int t_id;
    int n;
    double h;

};
double a=0;
double b=0;

int n = 1000000;
double total=0;
double total2=0;
double seg=1;
int next_task=0;
const int THREAD_NUM=8;
double func(double x){
    return 4*x*x*x+2*x*x+3*x;
}
void *Cal_pthread(void *param) {
    threadParam_t* p = (threadParam_t*)param;
    int task = 0;
    int n=p->n;
    double h=p->h;
    double x = 0;
    double currArea = 0;
    while (true) {
        pthread_mutex_lock(&barrier_mutex);
        next_task += 1;
        task = next_task;
        pthread_mutex_unlock(&barrier_mutex);
        if (task >= n )
            break;
        else {
            for (int i = task ; i < task+1; i++) {
                 x = a + i * h;
             currArea = func(x) * h;
            pthread_mutex_lock(&barrier_mutex);
            total += currArea;
            pthread_mutex_unlock(&barrier_mutex);
            }
        }
    }
    pthread_exit(nullptr);
     return nullptr;
}
double Cal(int n) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
        int thread_cnt = THREAD_NUM;
        pthread_t* thread_handles = (pthread_t*)malloc(thread_cnt * sizeof(pthread_t));
        threadParam_t* param = (threadParam_t*)malloc(thread_cnt * sizeof(threadParam_t));
        double h=(b-a)/n;
        total=(func(a)/2+func(b)/2)*h;
        for (int t_id = 0; t_id < thread_cnt; t_id++) {//分配任务
            param[t_id].t_id = t_id;//第几个线程
            param[t_id].n=n;
            param[t_id].h=h;

        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_create(&thread_handles[t_id], NULL,Cal_pthread, &param[t_id]);
        }

        for (int t_id = 0; t_id < thread_cnt; t_id++) {
            pthread_join(thread_handles[t_id], NULL);
        }
        free(thread_handles);
        free(param);
    QueryPerformanceCounter(&endTime);
     cout<<total<<endl;
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "普通Pthread方法耗时: " << time << "s" << endl;
    return time;


}
double Trap_static( double a, double b, int n)
{
total2 = 0.0;
    double h = (b - a) / n;
    double local_result = (func(a)/2+func(b)/2);
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    #pragma omp parallel for num_threads(THREAD_NUM) schedule(static) reduction(+:local_result)
    for (int i = 1; i < n; i++) {
        double local_x = a + i * h;
        local_result += func(local_x);
    }

    total2 = local_result * h ;
        cout<<total2<<endl;
        QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "oepnmp静态方法耗时: " << time << "s" << endl;
} /* Trap*/

double Trap_dynamic( double a, double b, int n)
{
total2 = 0.0;
    double h = (b - a) / n;
    double local_result = (func(a)/2+func(b)/2);
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    #pragma omp parallel for num_threads(THREAD_NUM) schedule(dynamic) reduction(+:local_result)
    for (int i = 1; i < n; i++) {
        double local_x = a + i * h;
        local_result += func(local_x);
    }

    total2 = local_result * h ;
        cout<<total2<<endl;
         QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "oepnmp动态dynamic方法耗时: " << time << "s" << endl;

}

double Trap_guided( double a, double b, int n)
{

    total2 = 0.0;
    double h = (b - a) / n;
    double local_result = (func(a)/2+func(b)/2);
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    #pragma omp parallel for num_threads(THREAD_NUM) schedule(guided) reduction(+:local_result)
    for (int i = 1; i < n; i++) {
        double local_x = a + i * h;
        local_result += func(local_x);
    }

    total2 = local_result * h ;
        cout<<total2<<endl;
         QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "oepnmp动态guided方法耗时: " << time << "s" << endl;
}

int main()

 {
        cout<<"输入积分区间"<<endl;
        cin>>a>>b;
//        h=(b-a)/n;
     Cal(n);
   Trap_guided( a,  b, n);
   Trap_static( a,  b, n);
    Trap_dynamic(a,b,n);
    return 0;
}





