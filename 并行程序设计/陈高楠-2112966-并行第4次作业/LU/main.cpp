#include <iostream>
#include <omp.h>
#include<tmmintrin.h>
#include<xmmintrin.h>
#include<emmintrin.h>
#include<pmmintrin.h>
#include<smmintrin.h>
#include<nmmintrin.h>
#include<immintrin.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include <thread>
using namespace std;
const int maxN = 10000;
const int N = 4;
const int THREAD_NUM = 8;
float A[maxN][maxN];
float b[maxN][maxN];
float c[maxN][maxN];
float d[maxN][maxN];
float answer[maxN];
float x[maxN];
double LU_omp_static(int n,float a[][maxN],int thread_num=8) {    //
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
       #pragma omp parallel for  num_threads(thread_num)  schedule(static)
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
//    cout << "LU_omp_static方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_guided(int n,float a[][maxN],int thread_num=8) {    //
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
       #pragma omp parallel for  num_threads(thread_num)  schedule(guided)
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
//    cout << "LU_omp_guided方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_dynamic(int n,float a[][maxN],int thread_num=8) {    //
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
       #pragma omp parallel for  num_threads(thread_num)  schedule(dynamic)
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
//    cout << "LU_omp_dynamic方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_static_sse(int n,float a[][maxN],int thread_num=8) {    //

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
        #pragma omp parallel for  num_threads(thread_num)  schedule(static)
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {

            float tem = a[j][i] / a[i][i];
            __m128 tem2=_mm_set1_ps(tem);
            for (int k = i + 1; k <= n; k+=4) //这个是列，列是n+1个（有b）
            {
                if(k+3>n)break;
                __m128 t1=_mm_loadu_ps(a[i]+k);
                __m128 t2=_mm_loadu_ps(a[j]+k);
                __m128 sub=_mm_sub_ps(t2,_mm_mul_ps(t1,tem2));
                _mm_storeu_ps(a[j]+k,sub);

            }
            for(int k=n-(n-i)%4+1;k<=n;k+=1){
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
    QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
//    cout << "LU_omp_static_sse方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_guided_sse(int n,float a[][maxN],int thread_num=8) {    //
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
        #pragma omp parallel for  num_threads(thread_num)  schedule(guided)
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {

            float tem = a[j][i] / a[i][i];
            __m128 tem2=_mm_set1_ps(tem);
            for (int k = i + 1; k <= n; k+=4) //这个是列，列是n+1个（有b）
            {
                if(k+3>n)break;
                __m128 t1=_mm_loadu_ps(a[i]+k);
                __m128 t2=_mm_loadu_ps(a[j]+k);
                __m128 sub=_mm_sub_ps(t2,_mm_mul_ps(t1,tem2));
                _mm_storeu_ps(a[j]+k,sub);

            }
            for(int k=n-(n-i)%4+1;k<=n;k+=1){
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
    QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
//    cout << "LU_omp_guided_sse方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_dynamic_sse(int n,float a[][maxN],int thread_num=8) {    //
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
       #pragma omp parallel for  num_threads(thread_num)  schedule(dynamic)
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {

            float tem = a[j][i] / a[i][i];
            __m128 tem2=_mm_set1_ps(tem);
            for (int k = i + 1; k <= n; k+=4) //这个是列，列是n+1个（有b）
            {
                if(k+3>n)break;
                __m128 t1=_mm_loadu_ps(a[i]+k);
                __m128 t2=_mm_loadu_ps(a[j]+k);
                __m128 sub=_mm_sub_ps(t2,_mm_mul_ps(t1,tem2));
                _mm_storeu_ps(a[j]+k,sub);

            }
            for(int k=n-(n-i)%4+1;k<=n;k+=1){
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
    QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
//    cout << "LU_omp_guided_sse方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_static_avx(int n,float a[][maxN],int thread_num=8) {    //
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
          #pragma omp parallel for  num_threads(thread_num)  schedule(static)
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {

            float tem = a[j][i] / a[i][i];
            __m256 tem2=_mm256_set1_ps(tem);
            for (int k = i + 1; k <= n; k+=8) //这个是列，列是n+1个（有b）
            {
                if(k+7>n)break;
                __m256 t1=_mm256_loadu_ps(a[i]+k);
                __m256 t2=_mm256_loadu_ps(a[j]+k);
                __m256 sub=_mm256_sub_ps(t2,_mm256_mul_ps(t1,tem2));
                _mm256_storeu_ps(a[j]+k,sub);

            }
            for(int k=n-(n-i)%8+1;k<=n;k+=1){
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
    QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
//    cout << "LU_omp_static_avx方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_guided_avx(int n,float a[][maxN],int thread_num=8,int chunk=1) {    //

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
         #pragma omp parallel for  num_threads(thread_num)  schedule(guided,chunk)
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {

            float tem = a[j][i] / a[i][i];
            __m256 tem2=_mm256_set1_ps(tem);
            for (int k = i + 1; k <= n; k+=8) //这个是列，列是n+1个（有b）
            {
                if(k+7>n)break;
                __m256 t1=_mm256_loadu_ps(a[i]+k);
                __m256 t2=_mm256_loadu_ps(a[j]+k);
                __m256 sub=_mm256_sub_ps(t2,_mm256_mul_ps(t1,tem2));
                _mm256_storeu_ps(a[j]+k,sub);

            }
            for(int k=n-(n-i)%8+1;k<=n;k+=1){
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
     QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
//    cout << "LU_omp_guided_avx方法耗时: " << time << "s" << endl;
    return time;
}
double LU_omp_dynamic_avx(int n,float a[][maxN],int thread_num=8) {    //

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
        #pragma omp parallel for  num_threads(THREAD_NUM)  schedule(dynamic)
        for (int j = i + 1; j < n; j++)//每一行的下面的行
        {

            float tem = a[j][i] / a[i][i];
            __m256 tem2=_mm256_set1_ps(tem);
            for (int k = i + 1; k <= n; k+=8) //这个是列，列是n+1个（有b）
            {
                if(k+7>n)break;
                __m256 t1=_mm256_loadu_ps(a[i]+k);
                __m256 t2=_mm256_loadu_ps(a[j]+k);
                __m256 sub=_mm256_sub_ps(t2,_mm256_mul_ps(t1,tem2));
                _mm256_storeu_ps(a[j]+k,sub);

            }
            for(int k=n-(n-i)%8+1;k<=n;k+=1){
                a[j][k] -= a[i][k] * tem;
            }
            a[j][i] = 0.00;
        }
    }
     QueryPerformanceCounter(&endTime);
    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
//    cout << "LU_omp_dynamic_avx方法耗时: " << time << "s" << endl;
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
double test(int choice=0,int n=128,int thread_num=8,int chunk=1){
    int x=0;
    switch(choice) {
        case 0:
            {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_static(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_static执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 1:
             {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_guided(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_guided执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 2:
             {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_dynamic(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_dynamic执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 3:
             {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_static_sse(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_static_sse执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 4:
             {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_guided_sse(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_guided_sse执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 5:
              {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_dynamic_sse(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_dynamic_sse执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 6:
              {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_static_avx(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_static_avx执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 7:
              {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_guided_avx(n,A,thread_num,chunk);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_guided_avx执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        case 8:
             {
                double alltime=0.0;
                while(x<20)
    {
        for (int i = 0; i < n; i++)
        for (int j = 0; j < n + 1; j++)
            A[i][j] = (rand() % 100000)/ 100.00;
         alltime+=LU_omp_dynamic_avx(n,A,thread_num);
         x++;
    }
        alltime/=20;
    cout<<"矩阵规模为"<<n<<" LU_omp_dynamic_avx执行时间:"<<alltime<<"s"<<endl;
            }


            break;
        default:
            std::cout << "Invalid choice" << std::endl;
    }






}



int main()
{
//int n=128;
//while(n<=2560){
//        test(0,n);
//         test(1,n);
//          test(2,n);
//           test(3,n);
//            test(4,n);
//             test(5,n);
//              test(6,n);
//               test(7,n);
//               test(8,n);
//    n+=128;
//}
//int c=5 ;
//while(c<=100){
//    cout<<c<<endl;
//        test(7,2560,8,c);
//    c+=5;
//}
//int t=2 ;
//while(t<=36){
//    cout<<t<<endl;
//        test(7,2560,t);
//    t+=2;
//}
test(7,2560,8,5);
}
