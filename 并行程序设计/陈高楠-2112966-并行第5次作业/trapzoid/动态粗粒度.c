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

int main(int argc, char* argv[]) {
    int my_rank, comm_sz, n = atoi(argv[1]);
    double a = 1, b = 100;
    double h = (b - a) / n;
    double global_result = 0.0;
    double start, end;
    int TASK_SIZE = atoi(argv[2]);
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (my_rank == 0) {
        start = MPI_Wtime();
        int tasks_completed = 0;
        double local_result;

        // 分发初始任务
        for (int i = 1; i < comm_sz; i++) {
            MPI_Send(&tasks_completed, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            tasks_completed+=TASK_SIZE;
        }

        while (tasks_completed < n) {
            MPI_Status status;
            MPI_Recv(&local_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            global_result += local_result;
            if (tasks_completed + TASK_SIZE > n) {
                MPI_Send(&tasks_completed, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                tasks_completed = n;  // 避免超出范围
            } else {
                MPI_Send(&tasks_completed, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                tasks_completed += TASK_SIZE;
            }
        }

        // 告诉所有进程任务已完成
        for (int i = 1; i < comm_sz; i++) {
            int stop_signal = -1;
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // 收集最后的任务
        for (int i = 1; i < comm_sz; i++) {
            MPI_Recv(&local_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_result += local_result;
        }

        end = MPI_Wtime();
    } else {
        while (1) {
            int task;
            MPI_Recv(&task, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (task == -1) break; // 停止信号

            double local_a = a + task * h;
            double local_b = local_a + h*TASK_SIZE;
            if(local_b>=b){
                local_b=b;
            double local_result = Trap(local_a, local_b, (local_b-local_a)/h+1, h);
            MPI_Send(&local_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
            else{
                double local_result = Trap(local_a, local_b, TASK_SIZE, h);
            MPI_Send(&local_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);}
        }
    }

    if (my_rank == 0) {
        printf("划分成小梯形的块数: %d\n", n);
        printf("计算结果是: %.15e\n", global_result);
        printf("总共耗时: %f s\n", end - start);
    }

    MPI_Finalize();
    return 0;
}
