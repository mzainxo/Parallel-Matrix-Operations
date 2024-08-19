#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    double **data;
    int rows;
    int cols;
} Matrix;

typedef struct {
    Matrix *matrix1;
    Matrix *matrix2;
    Matrix *result;
    int start_row;
    int end_row;
} ThreadData;

// Function to create a matrix
Matrix* createMatrix(int rows, int cols) {
    Matrix matrix = (Matrix)malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = (double*)malloc(rows * sizeof(double));
    for (int i = 0; i < rows; i++) {
        matrix->data[i] = (double*)malloc(cols * sizeof(double));
    }
    return matrix;
}

// Function to print a matrix
void printMatrix(Matrix *matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf(" %.2f ", matrix->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function to perform matrix addition
void matrixAddition(Matrix *matrix1, Matrix *matrix2, Matrix *result, int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < matrix1->cols; j++) {
            result->data[i][j] = matrix1->data[i][j] + matrix2->data[i][j];
        }
    }
}

// Function to perform matrix subtraction
void matrixSubtraction(Matrix *matrix1, Matrix *matrix2, Matrix *result, int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < matrix1->cols; j++) {
            result->data[i][j] = matrix1->data[i][j] - matrix2->data[i][j];
        }
    }
}

// Function to perform matrix multiplication
void matrixMultiplication(Matrix *matrix1, Matrix *matrix2, Matrix *result, int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < matrix2->cols; j++) {
            result->data[i][j] = 0;
            for (int k = 0; k < matrix1->cols; k++) {
                result->data[i][j] += matrix1->data[i][k] * matrix2->data[k][j];
            }
        }
    }
}

// Function to perform matrix inversion using Gauss-Jordan elimination
Matrix matrixInversion(Matrix *matrix) {
    int n = matrix->rows;

    Matrix augmented;
    augmented.rows = n;
    augmented.cols = 2 * n;
    augmented.data = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        augmented.data[i] = (double*)malloc(2 * n * sizeof(double));
    }

    // Create an augmented matrix [A | I]
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmented.data[i][j] = matrix->data[i][j];
        }
        for (int j = n; j < 2 * n; j++) {
            augmented.data[i][j] = (j == (i + n)) ? 1.0 : 0.0;
        }
    }

    // Perform Gauss-Jordan elimination
    for (int i = 0; i < n; i++) {
        double pivot = augmented.data[i][i];
        // Divide the current row by the pivot element
        for (int j = i; j < 2 * n; j++) {
            augmented.data[i][j] /= pivot;
        }
        // Eliminate other elements in the column
        for (int j = 0; j < n; j++) {
            if (j != i) {
                double factor = augmented.data[j][i];
                for (int k = i; k < 2 * n; k++) {
                    augmented.data[j][k] -= factor * augmented.data[i][k];
                }
            }
        }
    }

    // Extract the inverted matrix from the augmented matrix
    Matrix inverted;
    inverted.rows = n;
    inverted.cols = n;
    inverted.data = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        inverted.data[i] = (double*)malloc(n * sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inverted.data[i][j] = augmented.data[i][j + n];
        }
    }

    // Free memory
    for (int i = 0; i < n; i++) {
        free(augmented.data[i]);
    }
    free(augmented.data);

    return inverted;
}

// Thread function for matrix addition
void* matrixAdditionThread(void *arg) {
    ThreadData threadData = (ThreadData)arg;
    matrixAddition(threadData->matrix1, threadData->matrix2, threadData->result, threadData->start_row, threadData->end_row);
    pthread_exit(NULL);
}

// Thread function for matrix subtraction
void* matrixSubtractionThread(void *arg) {
    ThreadData threadData = (ThreadData)arg;
    matrixSubtraction(threadData->matrix1, threadData->matrix2, threadData->result, threadData->start_row, threadData->end_row);
    pthread_exit(NULL);
}

// Thread function for matrix multiplication
void* matrixMultiplicationThread(void *arg) {
    ThreadData threadData = (ThreadData)arg;
    matrixMultiplication(threadData->matrix1, threadData->matrix2, threadData->result, threadData->start_row, threadData->end_row);
    pthread_exit(NULL);
}

int main() {
    // Set random seed
    srand(time(NULL));
    int size[4];
	printf("\nENTER NUMBER OF ROWS FOR MATRIX A: ");
    scanf("%d",&size[0]);
    printf("ENTER NUMBER OF COLUMNS FOR MATRIX A: ");
    scanf("%d",&size[1]);
    printf("ENTER NUMBER OF ROW FOR MATRIX B:");
    scanf("%d",&size[2]);
    printf("ENTER NUMBER OF COLUMNS FOR MATRIX B: ");
    scanf("%d",&size[3]);
    // Create matrices
    Matrix *A = createMatrix(size[0], size[1]);
    Matrix *B = createMatrix(size[2], size[3]);
    
	double value;
	printf("\nENTER VALUES FOR MATRIX A\n");
	
	for(int r=0;r<size[0];r++)
	{
		for(int c=0;c<size[1];c++)
		{
			printf("in ROW %d and COLUMN %d: ",r,c);
			scanf("%lf",&value);
			A->data[r][c] = value;
		}
	}
	printf("\nENTER VALUES FOR MATRIX B\n");
	
	for(int r=0;r<size[2];r++)
	{
		for(int c=0;c<size[3];c++)
		{
			printf("in ROW %d and COLUMN %d: ",r,c);
			scanf("%lf",&value);
			B->data[r][c] = value;
		}
	}
    // Print matrices
    printf("\nMatrix A:\n");
    printMatrix(A);

    printf("\nMatrix B:\n");
    printMatrix(B);

    // Create result matrices
    Matrix *additionResult = createMatrix(A->rows, A->cols);
    Matrix *subtractionResult = createMatrix(A->rows, A->cols);
    Matrix *multiplicationResult = createMatrix(A->rows, B->cols);

    // Set number of threads
    int num_threads = 2;

    // Create thread data
    ThreadData threadData[num_threads];

    // Calculate rows per thread
    int rows_per_thread = A->rows / num_threads;

    // Create threads for matrix addition
    pthread_t additionThreads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        threadData[i].matrix1 = A;
        threadData[i].matrix2 = B;
        threadData[i].result = additionResult;
        threadData[i].start_row = i * rows_per_thread;
        threadData[i].end_row = (i == num_threads - 1) ? A->rows : (i + 1) * rows_per_thread;
        pthread_create(&additionThreads[i], NULL, matrixAdditionThread, &threadData[i]);
    }

    // Wait for addition threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(additionThreads[i], NULL);
    }

    // Create threads for matrix subtraction
    pthread_t subtractionThreads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        threadData[i].result = subtractionResult;
        pthread_create(&subtractionThreads[i], NULL, matrixSubtractionThread, &threadData[i]);
    }

    // Wait for subtraction threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(subtractionThreads[i], NULL);
    }

    // Create threads for matrix multiplication
    pthread_t multiplicationThreads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        threadData[i].matrix2 = B;
        threadData[i].result = multiplicationResult;
        pthread_create(&multiplicationThreads[i], NULL, matrixMultiplicationThread, &threadData[i]);
    }

    // Wait for multiplication threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(multiplicationThreads[i], NULL);
    }
	if(size[0] == size[2] && size[1] == size[3])
    {
		// addition and subtraction
		printf("\nMATRIX ADDITION:\n");
		printMatrix(additionResult);

		printf("\nMATRIX SUBTRACTION:\n");
		printMatrix(subtractionResult);
	}
	else
	{
		printf("\n\t\t***************** Matrices can't be added or subtracted *********************\n");
	}
	if(size[1] == size[2])
    {
		printf("MATRIX MULTIPLICATION:\n");
		printMatrix(multiplicationResult);
    }
    else
    {
    	printf("\n\t\t********************** Matrices can't be multiplied *************************\n");
    }
	int invA, invB;
	Matrix inverted = matrixInversion(A);
	Matrix inverted2 = matrixInversion(B);
	for(int r=0;r<size[0];r++)
	{
		for(int c=0;c<size[1];c++)
		{
			if(inverted.data[r][c] == INFINITY || inverted.data[r][c] == -INFINITY || 	 
			isnan(inverted.data[r][c]))
    		{
    			invA=1;
   			}
   			else if(invA != 1)
   			{
				invA=0;
    		}
		}
	}
	for(int r=0;r<size[2];r++)
	{
		for(int c=0;c<size[3];c++)
		{
			if(inverted2.data[r][c] == INFINITY || inverted2.data[r][c] == -INFINITY || 	 
			isnan(inverted2.data[r][c]))
    		{
    			invB=1;
   			}
   			else if(invB != 1)
   			{
				invB=0;
    		}
		}
	}
	if(invA == 1){
		printf("\n\t\t*********************** Matrix A is Non-Invertable ***************************\n");
	}
	else if(invA == 0){
		
		printf("\nINVERTED MATRIX A:\n");
		printMatrix(&inverted);
	}
	if(invB== 1){
		printf("\n\t\t*********************** Matrix B is Non-Invertable ***************************\n");
	}
	else if(invB == 0){
		
		printf("\nINVERTED MATRIX B:\n");
		printMatrix(&inverted2);
	}
    // Free memory
    for (int i = 0; i < A->rows; i++) {
        free(A->data[i]);
    }
    for (int i = 0; i < B->rows; i++) {
        free(B->data[i]);
    }
    for (int i = 0; i < additionResult->rows; i++) {
        free(additionResult->data[i]);
    }
    for (int i = 0; i < subtractionResult->rows; i++) {
        free(subtractionResult->data[i]);
    }
    for (int i = 0; i < multiplicationResult->rows; i++) {
        free(multiplicationResult->data[i]);
    }
    for (int i = 0; i < inverted.rows; i++) {
        free(inverted.data[i]);
    }
    for (int i = 0; i < inverted2.rows; i++) {
        free(inverted2.data[i]);
    }
    free(A->data);
    free(B->data);
    free(additionResult->data);
    free(subtractionResult->data);
    free(multiplicationResult->data);
    free(inverted.data);
    free(inverted2.data);
    free(A);
    free(B);
    free(additionResult);
    free(subtractionResult);
    free(multiplicationResult);

    return 0;
}
