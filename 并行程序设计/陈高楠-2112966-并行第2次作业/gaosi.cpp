#include <iostream>
#include <immintrin.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include <thread>

using namespace std;
const int maxN = 5000;
const int N = 8192;
float a[maxN][maxN];
float b[maxN][maxN];
float c[maxN][maxN];
float answer[maxN];
float answer2[maxN];
float answer3[maxN];
float x[maxN];
double LU(int n, float a[][maxN]) {
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
double LU_SSE(int n, float a[][maxN]) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    __m128 t1, t2, sub, tem2;
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
            tem2 = _mm_set1_ps(tem);
            for (int k = i + 1; k <= n; k += 4) //这个是列，列是n+1个（有b）

            {
                if (k + 3 > n)break;
                t1 = _mm_loadu_ps(a[i] + k);
                t2 = _mm_loadu_ps(a[j] + k);
                sub = _mm_sub_ps(t2, _mm_mul_ps(t1, tem2));
                _mm_storeu_ps(a[j] + k, sub);
            }
            for (int k = n - (n - i) % 4 + 1; k <= n; k += 1) {
                a[j][k] -= a[i][k] * tem;
            }

            a[j][i] = 0.00;


        }
    }
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "SSE高斯消元耗时: " << time << "s" << endl;
    return time;

}
double LU_AVX(int n, float a[][maxN]) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    __m256 t1, t2, sub, tem2;
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
            tem2 = _mm256_set1_ps(tem);
            for (int k = i + 1; k <= n; k += 8) //这个是列，列是n+1个（有b）

            {
                if (k + 7 > n)break;
                t1 = _mm256_loadu_ps(a[i] + k);
                t2 = _mm256_loadu_ps(a[j] + k);
                sub = _mm256_sub_ps(t2, _mm256_mul_ps(t1, tem2));
                _mm256_storeu_ps(a[j] + k, sub);
            }
            for (int k = n - (n - i) % 8 + 1; k <= n; k += 1) {
                a[j][k] -= a[i][k] * tem;
            }

            a[j][i] = 0.00;


        }
    }
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "AVX高斯消元耗时: " << time << "s" << endl;
    return time;

}
double generation_SSE(int n, float a[][maxN], float x[]) {

    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    __m128 t1, t2, t3, sub, tem;
    //转置


    for (int i = n - 1; i >= 0; i--) {
        x[i] = a[i][n];
        for (int j = i + 1; j < n; j += 4) {
            if (j + 3 >= n)break;
            t1 = _mm_load_ps(a[i] + j);
            t2 = _mm_load_ps(x + j);
            t3 = _mm_mul_ps(t1, t2);
            t3 = _mm_hadd_ps(t3, t3);
            t3 = _mm_hadd_ps(t3, t3);
            x[i] -= _mm_cvtss_f32(t3);

        }
        for (int j = n - (n - i - 1) % 4; j < n; j++) {

            x[i] -= a[i][j] * x[j];

        }




    }


    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "SSE方法优化回代: " << time << "s" << endl;
    return time;






}
double generation_AVX(int n, float a[][maxN], float x[]) {

    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    __m256 t1, t2, t3, tem;


    for (int i = n - 1; i >= 0; i--) {
        x[i] = a[i][n];
        for (int j = i + 1; j < n; j += 8) {
            if (j + 7 >= n)break;
            t1 = _mm256_load_ps(a[i] + j);
            t2 = _mm256_load_ps(x + j);
            t3 = _mm256_mul_ps(t1, t2);
            t3 = _mm256_hadd_ps(t3, t3);
            t3 = _mm256_hadd_ps(t3, t3);
            x[i] -= (_mm256_cvtss_f32(t3) + _mm256_cvtss_f32(_mm256_permute2f128_ps(t3, t3, 1)));

        }
        for (int j = n - (n - i - 1) % 8; j < n; j++) {

            x[i] -= a[i][j] * x[j];

        }




    }


    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "AVX方法优化回代: " << time << "s" << endl;
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
                    a[i][j] = (rand() % 100000) / 100.00;
                    b[i][j] = a[i][j];
                    c[i][j] = a[i][j];
                    answer[j] = 0.0;
                    answer2[j] = 0.0;
                    answer3[j] = 0.0;
                }

            }
            result[n / 128][0] += LU(n, a);
            result[n / 128][1] += LU_SSE(n, b);
            result[n / 128][2] += LU_AVX(n, c);
            result[n / 128][3] += generation(n, a, answer);
            result[n / 128][4] += generation_SSE(n, b, answer2);
            result[n / 128][5] += generation_AVX(n, c, answer3);


        }
        result[n / 128][0] /= 20;
        result[n / 128][1] /= 20;
        result[n / 128][2] /= 20;
        result[n / 128][3] /= 20;
        result[n / 128][4] /= 20;
        result[n / 128][5] /= 20;

        cout << result[n / 128][0] << "  " << result[n / 128][1] << "  " << result[n / 128][2] << "  " << result[n / 128][3] << "  " << result[n / 128][4] << "  " << result[n / 128][5] << endl;


        n += 128;
        cout << endl;

    }
    return 0;

}
