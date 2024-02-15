#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>


#define NUM_THREADS 8

float** A = NULL;
int N = 500;
int max(int a, int b) {
    return a > b ? a : b;
}

void A_init() {
    A = (float**)malloc(N * sizeof(float*));
    for (int i = 0; i < N; i++) {
        A[i] = (float*)malloc(N * sizeof(float));
    }

    for (int i = 0; i < N; i++) {
        A[i][i] = 1.0;
        for (int j = i + 1; j < N; j++) {
            A[i][j] = rand() % 5000;
        }
    }

    for (int k = 0; k < N; k++) {
        for (int i = k + 1; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] += A[k][j];
                A[i][j] = (int)A[i][j] % 5000;
            }
        }
    }
}

void A_initAsEmpty() {
    A = (float**)malloc(N * sizeof(float*));
    for (int i = 0; i < N; i++) {
        A[i] = (float*)malloc(N * sizeof(float));
        memset(A[i], 0, N * sizeof(float));
    }
}

void deleteA() {
    for (int i = 0; i < N; i++) {
        free(A[i]);
    }
    free(A);
}

void LU() {
    for (int k = 0; k < N; k++) {
        for (int j = k + 1; j < N; j++) {
            A[k][j] = A[k][j] / A[k][k];
        }
        A[k][k] = 1.0;
        for (int i = k + 1; i < N; i++) {
            for (int j = k + 1; j < N; j++) {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
    }
}

// 静态
double LU_mpi_static(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    double start_time = 0;
    double end_time = 0;
    int total = 0;
    int rank = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &total);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int begin = N / total * rank;
    int end = (rank == total - 1) ? N : N / total * (rank + 1);

    if (rank == 0) {
        A_init();
        for (j = 1; j < total; j++) {
            int b = j * (N / total), e = (j == total - 1) ? N : (j + 1) * (N / total);
            for (i = b; i < e; i++) {
                MPI_Send(&A[i][0], N, MPI_FLOAT, j, 1, MPI_COMM_WORLD);
            }
        }
    }
    else {
        A_initAsEmpty();
        for (i = begin; i < end; i++) {
            MPI_Recv(&A[i][0], N, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    for (k = 0; k < N; k++) {
        if ((begin <= k && k < end)) {
            for (j = k + 1; j < N; j++) {
                A[k][j] /= A[k][k];
            }
            A[k][k] = 1.0;
            for (j = 0; j < total; j++) {
                if (j != rank)
                    MPI_Send(&A[k][0], N, MPI_FLOAT, j, 0, MPI_COMM_WORLD);
            }
        }
        else {
            int src = (k < N / total * total) ? k / (N / total) : total - 1;
            MPI_Recv(&A[k][0], N, MPI_FLOAT, src, 0, MPI_COMM_WORLD, &status);
        }

        for (i = max(begin, k + 1); i < end; i++) {
            for (j = k + 1; j < N; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("静态块划分耗时：%.4lf s\n", end_time - start_time);
    }

    MPI_Finalize();

    return end_time - start_time;
}


// 动态
double LU_mpi_dynamic(int argc, char* argv[]) {
    double start_time = 0;
    double end_time = 0;
    MPI_Init(&argc, &argv);
    int total = 0;
    int rank = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &total);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        A_init();
        for (j = 1; j < total; j++) {
            for (i = j; i < N; i += total) {
                MPI_Send(&A[i][0], N, MPI_FLOAT, j, 1, MPI_COMM_WORLD);
            }
        }
    }
    else {
        A_initAsEmpty();
        for (i = rank; i < N; i += total) {
            MPI_Recv(&A[i][0], N, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    for (k = 0; k < N; k++) {
        if (k % total == rank) {
            for (j = k + 1; j < N; j++) {
                A[k][j] /= A[k][k];
            }
            A[k][k] = 1.0;

            for (j = 0; j < total; j++) {
                if (j != rank)
                    MPI_Send(&A[k][0], N, MPI_FLOAT, j, 0, MPI_COMM_WORLD);
            }
        }
        else {
            int src = k % total;
            MPI_Recv(&A[k][0], N, MPI_FLOAT, src, 0, MPI_COMM_WORLD, &status);
        }

        int begin = k;
        while (begin % total != rank)
            begin++;
        for (i = begin; i < N; i += total) {
            for (j = k + 1; j < N; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("动态块划分耗时：%.4lf s\n", end_time - start_time);
    }

    MPI_Finalize();

    return end_time - start_time;
}






int main(int argc, char* argv[]) {
    int task = 0;
    task = atoi(argv[1]);
    N = atoi(argv[2]);

    switch (task) {
    case 1:
        LU_mpi_static(argc, argv);
        break;
    case 2:
        LU_mpi_dynamic(argc, argv);
        break;
    }
}

