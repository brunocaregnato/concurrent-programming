#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define IMAX 100000
#define ERROR 1e-6
#define ERROR_BCG ERROR * ERROR

int N; //matrix dimensions 

void mat_show(double A[N][N]){
	for (int i=0; i<N; i++ ) {
		for (int j=0; j<N; j++ ) {
				printf("| %.16f | ", A[i][j]);
		}
        printf("\n");
	}
}

void vet_show(double v[N]){
	for (int i=0; i<N; i++)
        printf("| %.16f | ", v[i]);
        
    printf("\n");
}

void transposed_matrix(double A[][N], double B[][N]){ 
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
                B[i][j] = A[j][i]; 
        }
    }
} 

void subtraction_vectors(double v1[N], double v2[N], double result[N]) {
    for (int i = 0; i < N; i++) 
        result[i] = v1[i] - v2[i];
}

double vector_scalar_product(double v1[N], double v2[N]){
    double result = 0;
    for(int i = 0; i < N; i++)  
        result += v1[i] * v2[i];

    return result;
}

void product_matrix_by_vector(double A[N][N], double v[N], double result[N]) {    
    for (int i = 0; i < N; i++) {
        double s = 0;
        for (int j = 0; j < N; j++) {
            s += A[i][j] * v[j];
        }
        result[i] = s;
    }
}

void scalar_product_by_vector(double scalar, double v[N], double result[N]) {
    for (int i = 0; i < N; i++)
        result[i] = scalar * v[i];    
}

void sum_vectors(double v1[N], double v2[N], double result[N]) {
    for (int i = 0; i < N; i++)
        result[i] = v1[i] + v2[i];    
}

void bicg(double A[][N], double b[N], double result[N]){

    double *x = (double*) calloc(N, sizeof(double));
    double *x_result = (double*) calloc(N, sizeof(double));
    
    double A_transposed[N][N], r[N], r_result[N], r2[N], r2_result[N], vAux[N], vAux2[N];

    // A'
    transposed_matrix(A, A_transposed);

    // r = b - Ax
    product_matrix_by_vector(A, x, vAux);
    
    subtraction_vectors(b, vAux, r);
    
    memcpy(r2, r, N * sizeof(double));

    double rho = 1, rho0, v[N], beta, alpha;

    double *p = (double *) calloc(N, sizeof(double));
    double *p2 = (double *) calloc(N, sizeof(double));

    int i = 1;
    for(; i < IMAX; i++) {
        rho0 = rho;
                
        // rho = r2 * r 
        rho = vector_scalar_product(r2, r);
        beta = rho / rho0;

        // p = r + beta * p
        scalar_product_by_vector(beta, p, vAux);
        sum_vectors(r, vAux, p);

        // p2 = r2 + beta * p2
        scalar_product_by_vector(beta, p2, vAux);

        sum_vectors(r2, vAux, p2);
        
        // v = A * p
        product_matrix_by_vector(A, p, v);
        
        // alpha = rho / (p2 * v)
        alpha = rho/vector_scalar_product(p2, v);

        // x = x + alpha * p
        scalar_product_by_vector(alpha, p, vAux);
        sum_vectors(x, vAux, x_result);
        memcpy(x, x_result, N * sizeof(double));
        
        // r * r
        if(vector_scalar_product(r, r) < ERROR_BCG) break;  

        // r = r - alpha * v
        scalar_product_by_vector(alpha, v, vAux);
        subtraction_vectors(r, vAux, r_result);
        memcpy(r, r_result, N * sizeof(double));
        
        // r2 = r2 - alpha *A' * p2         
        product_matrix_by_vector(A_transposed, p2, vAux);    
        scalar_product_by_vector(alpha, vAux, vAux2);
        subtraction_vectors(r2, vAux2, r2_result);
        memcpy(r2, r2_result, N* sizeof(double));
    }

    printf("Iteracoes: %d\n", i);
    memcpy(result, x, N * sizeof(double));
}


int main(int argc, char **argv){
   
	if (argc != 2){
		printf("É necessário passar o valor para o tamanho da matriz!\n");
		exit(0);
	}

    N = atoi(argv[1]);  
    double A[N][N], b[N], result[N];
    
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] =  (double) rand() / (double) RAND_MAX;
        }
        b[i] = 1;
        result[i] = 0;
    }

    printf("Matriz A: \n");
    mat_show(A);

    bicg(A, b, result);
    product_matrix_by_vector(A, result, b);

    printf("Vetor final: ");
    vet_show(b);
    return 0;
}