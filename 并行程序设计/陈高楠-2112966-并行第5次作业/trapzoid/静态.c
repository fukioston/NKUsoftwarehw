#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>

//待积分的f(x)
double f(double x) {
    return 4 * x * x * x + 2 * x * x + 3 * x;
}

double Trap(double a, double b, int count, double h) {
    double my_result = (f(a) + f(b)) / 2.0;
    for (int i = 1; i < count; i++) {
        double x = a + i * h;
        my_result += f(x);
    }
    return my_result * h;
}
//计算局部和
double Trap(double a, double b, int count, double h) {
    double my_result, x;
    int i;
    my_result = (f(a) + f(b)) / 2.0;
    //局部求和，避免过多通信
    for (i = 1; i < count; i++) {
        x = a + i * h;
        my_result += f(x);
    }
    my_result *= h;
    return my_result;
}
//主函数
int main(int argc, char* argv[])
{
    int my_rank, comm_sz, n = atoi(argv[1]);
    double a = 1, b = 100;
    double h = (b - a) / n;
    double global_result;
    double start, end;
    double time;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
//printf("%li \n",my_rank);
    //0号进程开始计时
    if (my_rank == 0) {
       start = MPI_Wtime();
    }
    int local_n = n / comm_sz;
    double local_a = a + my_rank * local_n * h;
     double local_b = local_a + local_n * h;
    //将未除尽的任务数分配给最后一个进程
    if (my_rank == comm_sz - 1) {
        local_n = n - (comm_sz - 1) * local_n;
        local_b = b;
    }
    //每个进程计算自身分配的任务的局部和
    double local_result = Trap(local_a, local_b, local_n, h);
    //printf("%.15e\n",local_result);
    //其他进程将局部和发送给0号进程
    if (my_rank != 0) {
        MPI_Send(&local_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    //0号进程收集局部和并求全局和
    else {
        global_result = local_result;
        for (int source = 1; source < comm_sz; source++)
        {
            MPI_Recv(&local_result, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_result += local_result;
        }
        //完成计算，0号进程停止计时
        end = MPI_Wtime();
       time = end - start;
    }
    //0号进程打印出结果
    if (my_rank == 0) {
        printf("划分成小梯形的块数: %d\n", n);
        printf("计算结果是: %.15e\n", global_result);
        printf("总共耗时: %f s\n ", time);
    }

    MPI_Finalize();
    return 0;
}