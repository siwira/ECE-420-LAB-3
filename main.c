#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include "Lab3IO.h"
#include <math.h>
#include <omp.h>

/*
    We store matrix as contiguous 1D memory:
    A[row*(size+1) + col]
*/
#define MAT(A,row,col,size) A[(row)*((size)+1) + (col)]


// this function swaps two rows of a matrix A
// the row indices and the size of the matrix are parameters
void swapRows(double *A, int r1, int r2, int size){
    // r1 <--> r2
    for (int i = 0; i < size+1; ++i){ // size+1 because [A|b] has size+1 columns 
        double temp = MAT(A,r1,i,size);
        MAT(A,r1,i,size) = MAT(A,r2,i,size);
        MAT(A,r2,i,size) = temp;
    }
}


// this is a function that performs Gauss-Jordan Elimination with partial pivoting
// to solve linear systems of eqtns.
// don't worry about singularities as stated by the lab manual
void gaussianElimination(double *A, double *x, int size, int num_threads){

    // lets make the parallel region once!
    #pragma omp parallel num_threads(num_threads)
    {
        for (int cur_idx = 0; cur_idx < size; ++cur_idx){ // iterate by row

            int max_row_index = cur_idx;
            double local_max = 0.0;

            // 1. look for max element in the cur_idx'th column
            #pragma omp for nowait
            for (int row = cur_idx; row < size; ++row){
                double val = fabs(MAT(A,row,cur_idx,size));
                if (val > local_max){
                    local_max = val;
                    max_row_index = row;
                }
            }

            // combine local maximums safely
            #pragma omp critical
            {
                static double global_max;
                static int global_row;

                if (omp_get_thread_num() == 0){
                    global_max = 0.0;
                    global_row = cur_idx;
                }

                if (local_max > global_max){
                    global_max = local_max;
                    global_row = max_row_index;
                }

                max_row_index = global_row;
            }

            #pragma omp barrier

            // okay, we've found our next pivot element 
            // 2. we need to swap rows i and max_row_index
            #pragma omp single
            {
                swapRows(A, cur_idx, max_row_index, size);
            }

            #pragma omp barrier

            // 3. normalize all elements in cur_idx by the pivot
            double pivot;
            #pragma omp single
            pivot = MAT(A,cur_idx,cur_idx,size);

            #pragma omp for
            for (int col = cur_idx; col < size+1; ++col)
                MAT(A,cur_idx,col,size) /= pivot;


            // 4. now we need to eliminate all elements below the pivot in the cur_idx column
            // now we can parallelize here
            #pragma omp for schedule(static)
            for (int row = 0; row < size; ++row){
                if  (cur_idx == row) continue; // skip the pivot row

                double factor = MAT(A,row,cur_idx,size);

                double *current_row = &MAT(A,row,0,size);
                double *pivot_row   = &MAT(A,cur_idx,0,size);

                for (int col = cur_idx; col < size+1; ++col)
                    current_row[col] -= factor * pivot_row[col];
            }
        }

        // save output to x
        #pragma omp for
        for (int i = 0; i < size; i++){
            x[i] = MAT(A,i,size,size); // grab the final value in the last column 
        }
    }
}



int main (int argc, char* argv[]){
    double **A_input; // original augmented matrix [A|b]
    double *A;        // contiguous augmented matrix
    double *x;        // solution vector
    int size;
    int num_threads;
    double start, end, diff; // for timing

    // check for arguments first before anythig else
    if (argc != 2){
        printf("\nImproper usage. Run program as follows: ./main <num_threads>\n");
        return -1; // error
    }

    num_threads = atoi(argv[1]);

    // first load the data
    Lab3LoadInput(&A_input, &size);

    // set up the output matrix, x
    x = malloc(size * sizeof(double));

    // allocate contiguous matrix
    A = malloc(size * (size+1) * sizeof(double));

    // now A is the augmented matrix [A|b], with size rows and size+1 columns
    // copy from original layout to contiguous layout
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size+1; j++)
            MAT(A,i,j,size) = A_input[i][j];

    DestroyMat(A_input, size);

    // now start the Gaussian elimination
    GET_TIME(start);
    gaussianElimination(A, x, size, num_threads);  // perform the gaussian elimination, and store the result in x
    GET_TIME(end);

    diff = end - start;

    Lab3SaveOutput(x, size, diff);  // save the output and the time taken

    free(x);
    free(A);

    return 0;
}