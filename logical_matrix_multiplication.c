#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define N 1000 //matrix dimensions
#define P 2 //processes

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define buffer(i, j) buffer[i * N + j]

int max(int vet[N]) {
    int max = -1;
    for (int i = 0; i < N; i++) {
        if(max < vet[i]) max = vet[i];
    }

    return max;
}

void logical_matrix_multiplication(int *buffer, int line, int matrix1[][N], int matrix2[][N]) {
    int i, j, k;

    for (i = line; i < N; i+= P) {        
        for (j = 0; j < N; j++) {
            int minors[N];
            for (k = 0; k < N; k++) minors[k] = -1;            
            for (k = 0;k < N; k++) {
               if (minors[k] < min(matrix1[i][k], matrix2[k][j])) {
                   minors[k] = min(matrix1[i][k], matrix2[k][j]);
               }               
            }
            buffer(i, j) = max(minors);
        }
    }
}

void print_matrix(int *buffer) {
    int newLine = 0;
    printf("\n-------------- Final Matrix --------------\n");
    for (int i = 0; i < N * N; i++) {
        if (newLine == N) {
            newLine = 0;
            printf("\n");
        }
        printf("%d ", buffer[i]);
        newLine++;
    }
    printf("\n");
}

void main() {
    int matrix1 [N][N], matrix2[N][N];
    int i, j, shmid, sequenceId, pid, *buffer = NULL; 
    key_t key = 7;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            matrix1[i][j] = rand() % 10;
            matrix2[i][j] = rand() % 10;
        }
    }

    printf("\n-------------- Matrix 1 --------------\n");    

    for (i = 0; i < N; i++) {
       for (j = 0; j < N; j++) {
           printf("%d ", matrix1[i][j]);
       }
       printf("\n");
    }

    printf("\n-------------- Matrix 2 --------------\n");

    for (i = 0; i < N; i++) {
       for (j = 0; j < N; j++) {
           printf("%d ", matrix2[i][j]);
       }
       printf("\n");
    }

    shmid = shmget(key, sizeof(int) * N * N, 0600 | IPC_CREAT);
    buffer = shmat(shmid, 0 , 0 );

    sequenceId = 0;
    for(i = 1; i < P; i++){
        pid = fork();
        if (pid == 0) {
            sequenceId = i;
            break;
        } 
    }

    logical_matrix_multiplication(buffer, sequenceId, matrix1, matrix2);

    if (sequenceId != 0) {
        shmdt(buffer);
    }
    else {
        for(i = 1; i < P; i++) {
            wait(NULL);
        }    
        print_matrix(buffer);
    	shmdt(buffer);
	    shmctl(shmid, IPC_RMID, 0);
    }
}