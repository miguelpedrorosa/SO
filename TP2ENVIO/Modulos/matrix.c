#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/**
 * matrix_create
 *
 * Função para criar dinamicamente uma matriz com a dimensão de
 * nRows x nColumns
 *
 * Devolve um ponteiro para a matriz criada ou NULL em caso de erro
 */
Matrix *matrix_create(int nRows, int nColumns)
{
	Matrix *matrix = malloc(sizeof(Matrix));
	if (matrix == NULL)
		return NULL;

	matrix->nRows = nRows;
	matrix->nColumns = nColumns;

	// support an empty matrix -> nRowns = 0
	if (nRows == 0)
	{
		matrix->values = NULL;
		return matrix;
	}

	matrix->values = (int **)malloc(nRows * sizeof(int *));

	if (matrix->values == NULL)
	{
		matrix->nRows = 0; // set only for calling matrix_destroy
		matrix_destroy(matrix);
		return NULL;
	}

	for (int l = 0; l < nRows; ++l)
	{
		matrix->values[l] = malloc(nColumns * sizeof(int));

		if (matrix->values[l] == NULL)
		{
			matrix->nRows = l; // set only for calling matrix_destroy
			matrix_destroy(matrix);
			return NULL;
		}
	}

	return matrix;
}

/**
 * matrix_destroy
 *
 * Elimina os recursos da matriz passada em argumento
 */
void matrix_destroy(Matrix *matrix)
{
	if (matrix == NULL)
		return;

	for (int l = 0; l < matrix->nRows; ++l)
	{
		free(matrix->values[l]);
	}
	free(matrix->values);
	free(matrix);
}

void matrix_set_random(Matrix *matrix)
{
	for (int l = 0; l < matrix->nRows; ++l)
	{
		for (int c = 0; c < matrix->nColumns; ++c)
		{
			matrix->values[l][c] = random() % 1000;
		}
	}
}

void matrix_print(const Matrix *matrix)
{
	for (int l = 0; l < matrix->nRows; ++l)
	{
		for (int c = 0; c < matrix->nColumns; ++c)
		{
			printf("%04d ", matrix->values[l][c]);
		}
		printf("\n");
	}
}

/**
 * matrix_multiplication
 *
 * Calcula o produto entre as matrizes m1 e m2.
 * O numero de colunas de m1 tem de ser igual ao número de linhas de m2.
 * A matriz resultado tem o número de linhas de m1 e número de colunas de m2.
 *
 * M1     -> m x n
 * M2     -> n x p
 * result -> m x p
 *
 *                  n
 * result[i, j] =   S   a[i, k]*b[k, j]
 *                k = 0
 *             [ver https://en.wikipedia.org/wiki/Matrix_multiplication]
 *
 * Devolve a matriz resultado ou NULL em caso de erro
 */
Matrix *matrix_multiplication(const Matrix *m1, const Matrix *m2)
{
	if (m1->nColumns != m2->nRows)
		return NULL;

	Matrix *result = matrix_create(m1->nRows, m2->nColumns);
	if (result == NULL)
		return NULL;

	for (int i = 0; i < result->nRows; ++i)
	{
		for (int j = 0; j < result->nColumns; ++j)
		{

			int sum = 0;
			for (int k = 0; k < m1->nColumns; ++k)
			{
				sum += m1->values[i][k] * m2->values[k][j];
			}
			result->values[i][j] = sum;
		}
	}
	return result;
}

// Funções desenvolvidas

/**
 * Matrix *matrix_multiplication_with_processes(Matrix *m1, Matrix *m2, int nProcesses):
 * Esta função recebe duas matrizes m1 e m2 como entrada e o número de processos nProcessos a serem usados ​​para o cálculo.
 * Esta divide a primeira matriz m1 em sub-matrizes para nProcesses e atribui cada sub-matriz a um processo filho separado.
 * Os processos filhos executam a multiplicação de matrizes usando suas sub-matrizes atribuídas e a segunda matriz m2.
 * As submatrizes resultantes são enviadas de volta ao processo pai usando pipes, e o processo pai combina essas submatrizes para formar a matriz de resultado final.
 * Retorna a matrix_resultante em caso de sucesso e NULL ou EXIT_FAILURE em caso de falha.
 */

Matrix *matrix_multiplication_with_processes(Matrix *m1, Matrix *m2, int nProcesses)
{
	if (m1->nColumns != m2->nRows)
	{
		return NULL;
	}

	int **pipes;

	pipes = create_pipe(nProcesses);

	int nlines = matrix_get_rows(m1) / nProcesses;

	for (int i = 0; i < nProcesses; ++i)
	{
		pid_t pid = fork();
		if (pid < 0)
		{
			perror("Error: criação de processo");
			exit(EXIT_FAILURE);
		}

		if (pid == 0)
		{

			int initline = i * nlines;

			Matrix *ms = matrix_get_sub(m1, initline, nlines);
			if (ms == NULL)
			{
				perror("ERROR: creating sub matrix");
				exit(EXIT_FAILURE);
			}

			Matrix *mr = matrix_multiplication(ms, m2);
			if (mr == NULL)
			{
				perror("ERROR: matrix multiplication");
				exit(EXIT_FAILURE);
			}

			if (matrix_write(mr, pipes[i][1]) == -1)
			{
				perror("Failed to write matrix to the pipe");
				exit(EXIT_FAILURE);
			}
			matrix_destroy(ms);
			exit(0);
		}
	}

	Matrix *result = matrix_create(matrix_get_rows(m1), matrix_get_Columns(m2));
	if (result == NULL)
	{
		perror("ERROR: creating result matrix");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < nProcesses; ++i)
	{
		int initline = i * nlines;

		Matrix *mrp = matrix_read(pipes[i][0]);

		matrix_put_sub(result, mrp, initline, nlines);

		matrix_destroy(mrp);
	}

	for (int i = 0; i < nProcesses; i++)
	{
		close(pipes[i][1]);
		close(pipes[i][0]);
	}

	return result;
}

/*
Matrix *matrix_get_sub(Matrix *m, int line, int nlines):
Esta função retorna uma sub-matriz de m começando na linha e contendo nlinhas.
É usado para dividir a primeira matriz m1 em sub-matrizes nProcesses.
Retorna a matrix em caso de sucesso e NULL em caso de falha.
*/

Matrix *matrix_get_sub(Matrix *m, int line, int nlines)
{
	Matrix *matrix_sub = matrix_create(nlines, matrix_get_Columns(m));
	if (matrix_sub == NULL)
	{
		return NULL;
	}

	for (int initline = 0, ln = line; ln < line + nlines; ln++, initline++)
	{
		for (int cols = 0; cols < matrix_get_Columns(m); cols++)
		{
			matrix_sub->values[initline][cols] = m->values[ln][cols];
		}
	}

	return matrix_sub;
}

/*
int matrix_get_rows(Matrix *m):
Esta função retorna o número de linhas da matriz m
*/
int matrix_get_rows(Matrix *m)
{
	if (m == NULL)
	{
		return 0;
	}

	return m->nRows;
}

/*
int matrix_get_rows(Matrix *m):
Esta função retorna o número de colunas da matriz m
*/

int matrix_get_Columns(Matrix *m)
{
	if (m == NULL)
	{
		return 0;
	}
	return m->nColumns;
}

/*
int matrix_put_sub(Matrix *mDST, Matrix *mSRC, int initline, int nlines):
Esta função copia linhas nlines da matriz mSRC começando da linha inicial para a matriz mDST começando na sua primeira linha vazia.
É usado para enviar submatrizes do processo pai para os processos filhos.
Retorna 0 em caso de sucesso e -1 em caso de falha.
*/

int matrix_put_sub(Matrix *mDST, Matrix *mSRC, int initline, int nlines)
{
	if (mDST == NULL || mSRC == NULL)
	{
		return -1;
	}
	if (initline + nlines > mDST->nRows || mSRC->nColumns != mDST->nColumns)
	{
		return -1;
	}

	int row_dst = initline;
	for (int row_src = 0; row_src < nlines; row_src++, row_dst++)
	{
		for (int col = 0; col < mSRC->nColumns; col++)
		{
			mDST->values[row_dst][col] = mSRC->values[row_src][col];
		}
	}

	return 0;
}

/*
int matrix_save(Matrix *m, char filename[]):
Esta função salva a matriz m em um arquivo com o nome filename. Retorna 0 em caso de sucesso e -1 em caso de falha.
*/

int matrix_save(Matrix *m, char filename[])
{
	int fd = open(filename, O_WRONLY | O_CREAT, 0644);
	if (fd == -1)
	{
		perror("Failed to open file");
		return -1;
	}

	if (matrix_write(m, fd) == -1)
	{
		perror("Failed to write matrix to file");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

/*
Matrix *matrix_read(int fd):
função para ler uma matriz de um pipe
*/

Matrix *matrix_read(int fd)
{
	int rows, cols;
	if (read(fd, &rows, sizeof(int)) == -1 || read(fd, &cols, sizeof(int)) == -1)
	{
		perror("Failed to read matrix dimensions from pipe");
		return NULL;
	}

	Matrix *m = matrix_create(rows, cols);
	if (m == NULL)
	{
		perror("Failed to create matrix");
		return NULL;
	}

	for (int i = 0; i < rows; i++)
	{
		if (read(fd, m->values[i], cols * sizeof(int)) == -1)
		{
			perror("Failed to read matrix data from pipe");
			matrix_destroy(m);
			return NULL;
		}
	}

	return m;
}

/*
int matrix_write(Matrix *m, int fd)
função para escrever uma matriz em um pipe
Retorna 0 em caso de sucesso e -1 em caso de falha.
*/

int matrix_write(Matrix *m, int fd)
{
	if (write(fd, &m->nRows, sizeof(int)) == -1 || write(fd, &m->nColumns, sizeof(int)) == -1)
	{
		perror("Failed to write matrix dimensions to the pipe");
		return -1;
	}

	for (int i = 0; i < m->nRows; i++)
	{
		if (write(fd, m->values[i], sizeof(int) * m->nColumns) == -1)
		{
			perror("Failed to write matrix to the pipe");
			return -1;
		}
	}

	return 0;
}

/*
int **create_pipe(int nProcesses):
Essa função recebe um número inteiro nProcesses e cria uma matriz bidimensional de descritores de arquivo, representando o número de pipes de nProcesses.
A função usa a chamada de sistema pipe() para criar um par de descritores de arquivo para cada pipe e os armazena na matriz bidimensional.
*/

int **create_pipe(int nProcesses)
{

	int **pipes = (int **)malloc(nProcesses * sizeof(int *));
	for (int i = 0; i < nProcesses; i++)
	{
		pipes[i] = (int *)malloc(2 * sizeof(int));
		if (pipe(pipes[i]) < 0)
		{
			perror("Error: criação de pipe");
			exit(EXIT_FAILURE);
		}
	}

	return pipes;
}

/*
void matrix_set_value(Matrix *matrix,int n):
Esta função, coloca na matrix um determinado valor que resulta da soma de um inteiro n com 1
Esta foi utilizada para testar a multiplicação.
*/

void matrix_set_value(Matrix *matrix,int n)
{
	for (int l = 0; l < matrix->nRows; ++l)
	{
		for (int c = 0; c < matrix->nColumns; ++c)
		{
			matrix->values[l][c] =n+1;
		}
	}
}
