#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iohb.h>
#include <sys/time.h>
#include <unistd.h>

#define IMAX 100000
#define ERROR 1e-6
#define ERROR_BCG ERROR * ERROR

int dimension; //matrix dimensions 

void mat_show(double A[dimension][dimension]){
	for (int i=0; i<dimension; i++ ) {
		for (int j=0; j<dimension; j++ ) {
				printf("| %.16f | ", A[i][j]);
		}
        printf("\n");
	}
}

void vet_show(double v[dimension]){
	for (int i=0; i<dimension; i++)
        printf("| %.16f | ", v[i]);
        
    printf("\n");
}

void transposed_matrix(double A[][dimension], double B[][dimension]){ 
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
                B[i][j] = A[j][i]; 
        }
    }
} 

void subtraction_vectors(double v1[dimension], double v2[dimension], double result[dimension]) {
    for (int i = 0; i < dimension; i++) 
        result[i] = v1[i] - v2[i];
}

double vector_scalar_product(double v1[dimension], double v2[dimension]){
    double result = 0;
    for(int i = 0; i < dimension; i++)  
        result += v1[i] * v2[i];

    return result;
}

void product_matrix_by_vector(double A[][dimension], double v[dimension], double result[dimension]) {    
    for (int i = 0; i < dimension; i++) {
        double s = 0;
        for (int j = 0; j < dimension; j++) {
            s += A[i][j] * v[j];
        }
        result[i] = s;
    }
}

void scalar_product_by_vector(double scalar, double v[dimension], double result[dimension]) {
    for (int i = 0; i < dimension; i++)
        result[i] = scalar * v[i];    
}

void sum_vectors(double v1[dimension], double v2[dimension], double result[dimension]) {
    for (int i = 0; i < dimension; i++)
        result[i] = v1[i] + v2[i];    
}

void bicg(double A[][dimension], double b[dimension], double result[dimension]){

    double *x = (double*) calloc(dimension, sizeof(double));
    double *x_result = (double*) calloc(dimension, sizeof(double));
    
    double A_transposed[dimension][dimension], r[dimension], r_result[dimension], r2[dimension], r2_result[dimension], vAux[dimension], vAux2[dimension];

    // A'
    transposed_matrix(A, A_transposed);

    // r = b - Ax
    product_matrix_by_vector(A, x, vAux);
    
    subtraction_vectors(b, vAux, r);
    
    memcpy(r2, r, dimension * sizeof(double));

    double rho = 1, rho0, v[dimension], beta, alpha;

    double *p = (double *) calloc(dimension, sizeof(double));
    double *p2 = (double *) calloc(dimension, sizeof(double));

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
        memcpy(x, x_result, dimension * sizeof(double));
        
        // r * r
        if(vector_scalar_product(r, r) < ERROR_BCG) break;  

        // r = r - alpha * v
        scalar_product_by_vector(alpha, v, vAux);
        subtraction_vectors(r, vAux, r_result);
        memcpy(r, r_result, dimension * sizeof(double));
        
        // r2 = r2 - alpha *A' * p2         
        product_matrix_by_vector(A_transposed, p2, vAux);    
        scalar_product_by_vector(alpha, vAux, vAux2);
        subtraction_vectors(r2, vAux2, r2_result);
        memcpy(r2, r2_result, dimension* sizeof(double));
    }

    printf("Iteracoes: %d\n", i);
    memcpy(result, x, dimension * sizeof(double));
}

void read_matrix(char *file, int *M, int *dimension, int *nonzeros, int **columnPointer, int **lines, double **values){
	  
	int retorno, nrhs;
	char *tipo; 
	retorno = readHB_info(file, M, dimension, nonzeros, &tipo, &nrhs);
    
	if (!retorno){
        printf("Erro ao ler as informaçõess da matriz!\n");
		exit(-1);
	}
	
	*values = (double *) malloc (*nonzeros * sizeof(double));
	*lines  = (int *) malloc (*nonzeros * sizeof(int));
	*columnPointer  = (int *) malloc ((*dimension + 1) * sizeof(int));

	retorno = readHB_mat_double(file, *columnPointer, *lines, *values);
	     
	if (!retorno) {
        printf("Erro ao ler os valores da matriz!\n");
        exit(-1);
    }
}

int main(int argc, char **argv){
   
    if (argc != 2){
        printf ("%s < Necessario arquivo HB >\n", argv[0]);
        exit(0);
    }

    double *values = NULL;
    int *lines = NULL, *columnPointer = NULL;
    int M, N, nonzeros;
    
    read_matrix(argv[1], &M, &N, &nonzeros, &columnPointer, &lines, &values);
    dimension = M;

    double A[dimension][dimension], b[dimension], result[dimension];

    int t = 0;
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            if ((t + 1) < columnPointer[i + 1] && (j + 1) == lines[t]){
                A[j][i] = values[t];
                t++;
                continue;
            }
            A[j][i] = 0;
        }
        b[i] = 1;
        result[i] = 0;
    }


    printf("\nMatriz A: \n");
    mat_show(A);

    bicg(A, b, result);
    product_matrix_by_vector(A, result, b);

    printf("\nVetor final: ");
    vet_show(b);
    return 0;
}