#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARR_NUM 10000
#define ARR_LEN 10000
#define PROCESS_NUM 6
#define SEG (ARR_NUM / PROCESS_NUM)

int arr[ARR_NUM][ARR_LEN];

void init_2(void) {
    int i, j, ratio;
    srand((unsigned)time(NULL));
    for (i = 0; i < ARR_NUM; i++) {
        if (i < SEG) ratio = 0;
        else if (i < SEG * 2) ratio = 32;
        else ratio = 128;

        if ((rand() & 127) < ratio) {
            for (j = 0; j < ARR_LEN; j++) {
                arr[i][j] = ARR_LEN - j;
            }
        } else {
            for (j = 0; j < ARR_LEN; j++) {
                arr[i][j] = j;
            }
        }
    }
}
int compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

int main() {
    int my_rank, comm_sz;
    double start, end;
    double time;
    
    // Initialize the array
    init_2();

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   if (my_rank == 0) {
        start = MPI_Wtime();
        int tasks_completed = 0;
        double local_result=0;
        // 分发初始任务
        for (int i = 1; i < comm_sz; i++) {
            MPI_Send(&tasks_completed, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            tasks_completed++;
        }

        while (tasks_completed <  ARR_NUM) {
            MPI_Status status;
            MPI_Recv(&local_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Send(&tasks_completed, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            tasks_completed++;
        }

        // 告诉所有进程任务已完成
        for (int i = 1; i < comm_sz; i++) {
            int stop_signal = -1;
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // 收集最后的任务
        for (int i = 1; i < comm_sz; i++) {
            MPI_Recv(&local_result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        end = MPI_Wtime();
        time=end-start;
        // printf("各进程运行时间如下:\n");
        printf("0号进程: %f s\n", time);
        // for (int source = 1; source < comm_sz; source++) {
        //     MPI_Recv(&time, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //     printf("%d号进程: %f s\n", source, time);
        // }
    }
    else {
        double start1=MPI_Wtime();
        while (1) {

            int task;
            double local_result=0;
            MPI_Recv(&task, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (task == -1) break; // 停止信号
            qsort(arr[task], ARR_LEN, sizeof(int), compare_ints);
            MPI_Send(&local_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
         double end1 = MPI_Wtime();
        double time1=end1-start1;
        printf("%d号进程: %f s\n", my_rank, time1);
        //  MPI_Send(&time1, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    
    MPI_Finalize();
    return 0;
}