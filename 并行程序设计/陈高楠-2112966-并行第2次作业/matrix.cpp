#include <iostream>
#include <pmmintrin.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
using namespace std;
const int maxN = 10000;
const int T = 32;
float a[maxN][maxN];
float b[maxN][maxN];
float c[maxN][maxN];
long long head, tail, freq;
int n;
double mul(int n, float a[][maxN], float b[][maxN], float c[][maxN]) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = 0.0;
            for (int k = 0; k < n; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "串行算法耗时: " << time << "s" << endl;
    return time;

}
double trans_mul(int n, float a[][maxN], float b[][maxN], float c[][maxN]) { // Cache优化 O(n) = n^3
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = 0.0;
            for (int k = 0; k < n; ++k) {
                c[i][j] += a[i][k] * b[j][k];
            }
        }
    } // transposition
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);

    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "Cache优化耗时: " << time << "s" << endl;
    return time;
}
double sse_mul(int n, float a[][maxN], float b[][maxN], float c[][maxN]) { //SSE版本 O(n) = (n^3)/4
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);

    __m128 t1, t2, sum;  //__m128 == float
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i][j] = 0.0;
            sum = _mm_setzero_ps();  //Initialize
            for (int k = n - 4; k >= 0; k -= 4) { // sum every 4 elements 
                t1 = _mm_loadu_ps(a[i] + k);
                t2 = _mm_loadu_ps(b[j] + k);
                t1 = _mm_mul_ps(t1, t2);
                sum = _mm_add_ps(sum, t1);
            }
            sum = _mm_hadd_ps(sum, sum);
            sum = _mm_hadd_ps(sum, sum);
            _mm_store_ss(c[i] + j, sum);
            for (int k = (n % 4) - 1; k >= 0; --k) { // handle the last n%4elements
                c[i][j] += a[i][k] * b[j][k];
            }
        }
    }
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);

    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "SSE版本耗时: " << time << "s" << endl;
    return time;
}
double sse_tile(int n, float a[][maxN], float b[][maxN], float c[][maxN], int T) {
    LARGE_INTEGER freq;
    LARGE_INTEGER beginTime;
    LARGE_INTEGER endTime;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&beginTime);

    __m128 t1, t2, sum;
    float t;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
    for (int r = 0; r < n / T; ++r)
        for (int q = 0; q < n / T; ++q) {
            for (int i = 0; i < T; ++i)
                for (int j = 0; j < T; ++j)
                    c[r * T + i][q * T + j] = 0.0;
            for (int p = 0; p < n / T; ++p) {
                for (int i = 0; i < T; ++i)
                    for (int j = 0; j < T; ++j) {
                        sum = _mm_setzero_ps();
                        for (int k = T-4; k >=0; k -= 4) {
                            t1 = _mm_loadu_ps(a[r * T + i] + p * T + k);
                            t2 = _mm_loadu_ps(b[q * T + j] + p * T + k);
                            t1 = _mm_mul_ps(t1, t2);
                            sum = _mm_add_ps(sum, t1);
                        }
                        sum = _mm_hadd_ps(sum, sum);
                        sum = _mm_hadd_ps(sum, sum);
                        _mm_store_ss(&t, sum);
                        c[r * T + i][q * T + j] += t;
                        for (int k = (T % 4) - 1; k >= 0; --k) { // handle the last n%4elements
                            c[r * T + i][q * T + j] += a[r * T + i][p * T + k] * b[q * T + j][p * T + k];
                        }

                    }
            }
        }
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);

    QueryPerformanceCounter(&endTime);

    double time = (double)(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart;
    cout << "分片策略耗时: " << time << "s" << endl;
    return time;
}
int main()
{
    /*n = 128;
    int guimo = 200;
    int x = 0;
    double result[100][4];
    for (int i = 0;i < 100;i++) {
        for (int j = 0;j < 4;j++)
            result[i][j] = 0;
    }
    while (n <= 2048)
    {

        cout << "矩阵规模为" << n << "*" << n << "时" << endl;
        x = 0;

        while (x++ < 20)
        {
            cout << "第" << x << "次矩阵乘法" << endl;
            cout << endl;
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++) {
                    a[i][j] = rand();
                    b[i][j] = rand();
                }

            }
            result[n / 128][0] += mul(n, a, b, c);
            result[n / 128][1] += trans_mul(n, a, b, c);
            result[n / 128][2] += sse_mul(n, a, b, c);
            result[n / 128][3] += sse_tile(n, a, b, c, T);

            cout << endl;
        }
        result[n / 128][0] /= 20;
        result[n / 128][1] /= 20;
        result[n / 128][2] /= 20;
        result[n / 128][3] /= 20;
        cout << result[n / 128][0] << "  " << result[n / 128][1] << "  " << result[n / 128][2] << "  " << result[n / 128][3] << endl;


        n += 128;
        cout << endl;

    }*/

    n = 1536;
    double result2[100];
    for (int j = 0;j <= 10;j++)
        result2[j] = 0;
    for (int i = 0;i < 20;i++)
    {
        
        result2[0] += sse_tile(n, a, b, c, 2);
        result2[0] += sse_tile(n, a, b, c, 4);
        result2[1] += sse_tile(n, a, b, c, 8);
        result2[2] += sse_tile(n, a, b, c, 16);
        result2[3] += sse_tile(n, a, b, c, 32);
        result2[4] += sse_tile(n, a, b, c,48);
        result2[5] += sse_tile(n, a, b, c, 64);
        result2[6] += sse_tile(n, a, b, c, 128);
        result2[7] += sse_tile(n, a, b, c, 256);
        result2[8] += sse_tile(n, a, b, c, 512);

        
        result2[9] += sse_mul(n, a, b, c);
        cout << endl;



    }
    result2[0] /= 20;
    result2[1] /= 20;
    result2[2] /= 20;
    result2[3] /= 20;
    result2[4] /= 20;
    result2[5] /= 20;
    result2[6] /= 20;
    result2[7] /= 20;
    result2[8] /= 20;
    result2[9] /= 20;
    cout << result2[0] << "  " << result2[1] << "  " << result2[2] << "  " << result2[3] << " " << result2[4] << " " << " " << result2[5] << " " << result2[6]<<"  "<< result2[7] << "  " << result2[8] << "  " << result2[9] << endl;

    return 0;


}

