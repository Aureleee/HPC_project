#include "header.h"

/* same partition idea: base + rem on first threads */
void omp_partition(int n, int tid, int nthreads, int *start, int *count)
{
	int base;
	int rem;

	base = n / nthreads;
	rem = n % nthreads;
	*start = tid * base + (tid < rem ? tid : rem);
	*count = base + (tid < rem);
}

/* parallel matvec, each thread writes its chunk into y */
void omp_tridiag_matvec(const t_vector *lower, const t_vector *diag,
					const t_vector *upper, const t_vector *b,
					t_vector *y)
{
	int n;

	n = diag->size;
	#pragma omp parallel
	{
		int tid = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		int start;
		int count;

		omp_partition(n, tid, nthreads, &start, &count);
		if (count > 0)
			omp_compute_chunk(lower, diag, upper, b, y, start, count);
	}
}
