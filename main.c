#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include "Lab3IO.h"

void gaussianElimination(double **A, int size){
    
}

void swapRows(double **A, int r1, int r2, int size){
    int i;
    double temp;
    for (i = 0; i < size; ++i){
        temp = A[r1][i];
        A[r1][i] = A[r2][i];
        A[r2][i] = temp;
    }
}

int main (int argc, char* argv[]){
    double **A; // augmented matrix [A|b]
    double *x; // solution vector
    int size;
    double start, end, diff; // for timing
    //set up the output matrix, x
    x = malloc(size * sizeof(double));
    // first load the data
    Lab3LoadInput(&A, &size);
    // now A is the augmented matrix [A|b], with size rows and size+1 columns
    // now start the Gaussian elimination
    GET_TIME(start);
    gaussianElimination(A, size);  // perform the gaussian elimination, and store the result in x
    GET_TIME(end);
    diff = end - start;
    Lab3SaveOutput(x, size, diff);  //save the output and the time taken
    free(x);
    DestroyMat(A, size);
    return 0;
}

}