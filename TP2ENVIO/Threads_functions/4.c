#include "4_Functions.h"

int main(){
    int nRows = 10;
    int nColumns = 10;
    Matrix *matrix= matrix_create(nRows,nColumns);

    matrix_set_random(matrix);

    row_matrix_sum(matrix);

    matrix_destroy(matrix);

    return 0;
}