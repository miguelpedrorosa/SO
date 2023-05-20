/**
 * Matrix.h
 * 
 * Funções relacionadas com a manipulação de matrizes
 * 
 **/
#ifndef _MATRIX_H_
#define _MATRIX_H_



/**
 * Estrutura que representa uma matrix
 */
typedef struct 
{
	int nRows;
	int nColumns; 
	int **values;
} Matrix;


Matrix *matrix_create         (int nRows, int nColumns);

void    matrix_destroy        (Matrix *matrix);

void    matrix_set_value      (Matrix *matrix, int value);

void    matrix_set_random     (Matrix *matrix);

void    matrix_print          (const Matrix *matrix);

Matrix *matrix_multiplication (const Matrix *m1, const Matrix *m2);

//Funções desenvolvidas

Matrix *matrix_multiplication_with_processes(Matrix *m1, Matrix *m2, int nProcesses);

Matrix *matrix_get_sub(Matrix *m, int line, int nlines);

int matrix_get_rows(Matrix *m);

int matrix_get_Columns(Matrix *m);

int matrix_put_sub(Matrix *mDST,Matrix *mSRC, int initline , int nlines );

int matrix_save(Matrix *m , char filename[]);

Matrix *matrix_read(int fd);

int matrix_write(Matrix *m, int fd);

int **create_pipe(int nProcesses);

void matrix_set_value(Matrix *matrix,int n);

#endif
