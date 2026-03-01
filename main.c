#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include "Lab3IO.h"
#include <math.h>

// this function swaps two rows of a matrix A
// the row indices and the size of the matrix are parameters
void swapRows(double **A, int r1, int r2, int size){
    // r1 <--> r2
    int i;
    double temp;
    for (i = 0; i < size+1; ++i){ // size+1 because [A|b] has size+1 columns 
        temp = A[r1][i];
        A[r1][i] = A[r2][i];
        A[r2][i] = temp;
    }
}
// this is a function that performs Gauss-Jordan Elimination with partial pivoting
// to solve linear systems of eqtns.
// don't worry about singularities as stated by the lab manual
void gaussianElimination(double **A, double *x, int size, int num_threads){
    int max_row_index;
    // look for the largest element in each column in a for loop
    // this will be the first step to finding the pivot row
    for (int cur_idx = 0; cur_idx < size; ++cur_idx){ // iterate by row
        max_row_index = cur_idx; // start at the highest remaining row in the matrix
        // 1. look for max element in the cur_idx'th column
        for (int row = cur_idx+1; row < size; ++row){
            if (fabs(A[row][cur_idx]) > fabs(A[max_row_index][cur_idx])) // changed to fabs()
                max_row_index = row;
        }
        // okay, we've found our next pivot element 
        //2. we need to swap rows i and max_row_index
        swapRows(A, cur_idx, max_row_index, size);
        
        // 3. normalize all elements in cur_idx by the pivot
        double pivot = A[cur_idx][cur_idx];
        for (int col = cur_idx; col < size+1; ++col)
            A[cur_idx][col] /= pivot;
        // 4. now we need to eliminate all elements below the pivot in the cur_idx column
        for (int row = 0; row < size; ++row){
            if  (cur_idx == row) continue; // skip the pivot row
            double factor = A[row][cur_idx]; // this is the factor we need to multiply the pivot row by to eliminate the current element
            for (int col = cur_idx; col < size+1; ++col) // start at cur_idx for simplicity
                A[row][col] -= factor * A[cur_idx][col];
        }
    }
    // save output to x
    for (int i = 0; i < size; i++){
        x[i] = A[i][size]; // grab the final value in the last column 
    }
}



int main (int argc, char* argv[]){
    double **A; // augmented matrix [A|b]
    double *x; // solution vector
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
    Lab3LoadInput(&A, &size);
    //set up the output matrix, x
    x = malloc(size * sizeof(double));
    // now A is the augmented matrix [A|b], with size rows and size+1 columns
    // now start the Gaussian elimination
    GET_TIME(start);
    gaussianElimination(A, x, size, num_threads);  // perform the gaussian elimination, and store the result in x
    GET_TIME(end);
    diff = end - start;
    Lab3SaveOutput(x, size, diff);  //save the output and the time taken
    free(x);
    DestroyMat(A, size);
    return 0;
}
