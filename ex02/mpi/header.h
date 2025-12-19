#ifndef HEADER_H
#define HEADER_H

/* one header only. simple project so it's ok */

#include <mpi.h>

#include <stddef.h>

typedef struct s_vector
{
	int size;
	double *vec;
} t_vector;

/* vector_utils.c */
int		vector_alloc(t_vector *v, int n);
void	vector_free(t_vector *v);
void	vector_fill_constant(t_vector *v, double x);
void	vector_fill_random(t_vector *v, unsigned int *seed);
void	vector_print(const char *name, const t_vector *v, int max_elems);

/* mpi_core.c */
void	mpi_partition(int n, MPI_Comm comm, int *start, int *count);
void	mpi_counts_displs(int n, MPI_Comm comm, int *counts, int *displs);
void	mpi_allgatherv_double(const double *sendbuf, int sendcount,
						double *recvbuf, int n, MPI_Comm comm);
void	mpi_gatherv_double_root(const double *sendbuf, int sendcount,
						double *recvbuf, int n, int root, MPI_Comm comm);

/* math_ops.c */
void	mpi_compute_chunk(const t_vector *lower, const t_vector *diag,
					const t_vector *upper, const t_vector *b_full,
					double *y_local, int start, int count);
void	tridiag_matvec_serial(const t_vector *lower, const t_vector *diag,
					const t_vector *upper, const t_vector *b,
					t_vector *y);

#endif