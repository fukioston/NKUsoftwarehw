#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <immintrin.h>
#include <windows.h>
#include <pthread.h>
using namespace std;
typedef struct {
	int threadId;
	vector<int>* a;
	int chunk_size;
} threadParm_t;
const int ARR_NUM = 10000;
const int ARR_LEN = 10000;
const int THREAD_NUM =8;
const int BLOCK_SIZE=50;
const int TOTAL_BLOCKS=(ARR_NUM/BLOCK_SIZE)*(ARR_NUM/BLOCK_SIZE);
const int seg = ARR_NUM / THREAD_NUM;
vector<int> arr[ARR_NUM];
vector<int> arr2[ARR_NUM];
pthread_mutex_t mutex;
long long head, freq; // timers
void init(void)
{
	srand(unsigned(time(NULL)));
	for (int i = 0; i < ARR_NUM; i++) {
		arr[i].resize(ARR_LEN);
		arr2[i].resize(ARR_LEN);
		for (int j = 0; j < ARR_LEN; j++)
			{arr[i][j] = rand();
			arr2[i][j]=arr[i][j];
			}
	}
}
void init_2(void)
{
	int ratio;
	srand(unsigned(time(NULL)));
	for (int i = 0; i < ARR_NUM; i++) {
		arr[i].resize(ARR_LEN);
		arr2[i].resize(ARR_LEN);
		if (i < seg) ratio = 0;
		else if (i < seg * 2) ratio = 32;
		else if (i < seg * 3) ratio = 64;
		else ratio = 128;
		if ((rand() & 127) < ratio)
			for (int j = 0; j < ARR_LEN; j++)
				{
                arr[i][j] = ARR_LEN - j;
				arr2[i][j]=arr[i][j];
				}

		else
			for (int j = 0; j < ARR_LEN; j++)
				{arr[i][j] = j;
				arr2[i][j]=arr[i][j];
				}
	}
}
void* arr_sort(void* parm)
{
	threadParm_t* p = (threadParm_t*)parm;
	int r = p->threadId;
	long long tail;
	for (int i = r * seg; i < (r + 1) * seg; i++)
		sort(arr[i].begin(), arr[i].end());
	pthread_mutex_lock(&mutex);
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);
	printf("Thread %d: %lfs.\n", r, (tail - head)* 1000.0 / freq);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
int next_arr = 0;
pthread_mutex_t mutex_task;
void* arr_sort_fine(void* parm)
{
	threadParm_t* p = (threadParm_t*)parm;
	int r = p->threadId;
	int s=p->chunk_size;
	int task = 0;
	long long tail;
	while (1) {
		pthread_mutex_lock(&mutex_task);
		task = next_arr;
		next_arr+=s;
		pthread_mutex_unlock(&mutex_task);
		if (task >= ARR_NUM) break;
		int end_task=task+s;
		if (end_task > ARR_NUM) end_task = ARR_NUM;
		for (int i = task; i < end_task; i++) {
            stable_sort(arr[i].begin(),arr[i].end());
        }
	}
	pthread_mutex_lock(&mutex);
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);
//	printf("Thread %d: %lfs.\n", r, (tail - head)* 1000.0  / freq);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);

}



int main(int argc, char* argv[])
{
//	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
//	init_2();
//	mutex = PTHREAD_MUTEX_INITIALIZER;
//	pthread_t thread[THREAD_NUM];
//	threadParm_t threadParm[THREAD_NUM];
//	QueryPerformanceCounter((LARGE_INTEGER*)&head);
//	for (int i = 0; i < THREAD_NUM; i++)
//	{
//		threadParm[i].threadId = i;
//		pthread_create(&thread[i], NULL, arr_sort_fine, (void*)&threadParm[i]);
//	}
//	for (int i = 0; i < THREAD_NUM; i++)
//	{
//		pthread_join(thread[i], NULL);
//	}
//	pthread_mutex_destroy(&mutex);

QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
long long tail;
int x=0;
int x2=0;
double t=0.0;
double t1=0.0;
double t2=0.0;
while(x2++<100)
{

    t2=0;
    x=0;
    while(x++<8)
{
init_2();

next_arr = 0;
mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread[THREAD_NUM];
threadParm_t threadParm[THREAD_NUM];
QueryPerformanceCounter((LARGE_INTEGER *)&head);
for (int i = 0; i < THREAD_NUM; i++)
{
threadParm[i].threadId = i;
threadParm[i].a=arr;
threadParm[i].chunk_size=x2;
pthread_create(&thread[i], NULL, arr_sort_fine, (void *)&threadParm[i]);
}
	for (int i = 0; i < THREAD_NUM; i++)
{
pthread_join(thread[i], NULL);
}
pthread_mutex_destroy(&mutex);
QueryPerformanceCounter((LARGE_INTEGER*)&tail);
t=(tail - head) * 1000.0 / freq;
//printf("总时间: %lfms.\n", t);
t2+=t;
}

//cout<<"静态"<<t1/20;
cout<<"chuck:"<<x2<<" ";
cout<<"时间:"<<t2/8;
cout<<endl;
}

}
