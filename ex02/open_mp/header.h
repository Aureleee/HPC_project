#ifndef HEADER_H
#define HEADER_H

#include <omp.h>

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

/* math_ops.c */
void	omp_compute_chunk(const t_vector *lower, const t_vector *diag,
					const t_vector *upper, const t_vector *b,
					t_vector *y, int start, int count);
void	tridiag_matvec_serial(const t_vector *lower, const t_vector *diag,
					const t_vector *upper, const t_vector *b,
					t_vector *y);

/* omp_core.c */
void	omp_partition(int n, int tid, int nthreads, int *start, int *count);
void	omp_tridiag_matvec(const t_vector *lower, const t_vector *diag,
					const t_vector *upper, const t_vector *b,
					t_vector *y);

#endif
